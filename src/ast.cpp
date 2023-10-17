#include "ast.h"

#include <fstream>
#include <ostream>

#include "error.h"
#include "cli.h"

namespace AST
{
    Value *logError(std::string message)
    {
        ErrorHandler::error(message);
        return nullptr;
    }

    Function *getFunction(std::string Name)
    {
        // First, see if the function has already been added to the current module.
        if (auto *F = MasterAST::TheModule->getFunction(Name))
            return F;

        // If not, check whether we can codegen the declaration from some existing
        // prototype.
        auto FI =  MasterAST::FunctionProtos.find(Name);
        if (FI !=  MasterAST::FunctionProtos.end())
            return FI->second->codegen();

        // If no existing prototype exists, return null.
        return nullptr;
    }

    void MasterAST::initializeJIT()
    {
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();
        InitializeNativeTargetAsmParser();

        TheJIT = ExitOnErr(orc::JIT::Create());
    }

    void MasterAST::initializeModule()
    {
        TheContext = new LLVMContext;
        TheModule = new Module(CompilerOptions::SourceFileLocation, *TheContext);
        TheModule->setDataLayout(TheJIT->getDataLayout());

        // Create a new builder for the module.
        Builder = new IRBuilder<>(*TheContext);

        TheFPM = new legacy::FunctionPassManager(TheModule);

        // Do simple "peephole" optimizations and bit-twiddling optzns.
        TheFPM->add(createInstructionCombiningPass());
        // Reassociate expressions.
        TheFPM->add(createReassociatePass());
        // Eliminate Common SubExpressions.
        TheFPM->add(createGVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        TheFPM->add(createCFGSimplificationPass());

        TheFPM->doInitialization();
    }

    void MasterAST::outputModule()
    {
        std::string ir;
        raw_string_ostream ostream(ir);
        ostream << *TheModule;
        ostream.flush();

        std::ofstream irFile;
        irFile.open(CompilerOptions::OutputLL);
        irFile << ir;
        irFile.close();

        if (system(("llc " + CompilerOptions::OutputLL + " -o " + CompilerOptions::OutputS).c_str()) != 0)
        {
            std::cout << "Error compiling LLVM IR." << std::endl;
            return;
        }
        if (system(("gcc -c " + CompilerOptions::OutputS + " -o " + CompilerOptions::OutputO).c_str()) != 0)
        {
            std::cout << "Error compiling assembly." << std::endl;
            return;
        }
        if (!CompilerOptions::IsLibrary)
        {
            if (system(("gcc " + CompilerOptions::OutputO + " -o " + CompilerOptions::OutputFinal + " -no-pie").c_str()) != 0)
            {
                std::cout << "Error compiling object file." << std::endl;
                return;
            }
        }
    }

    void MasterAST::removeBuildFiles()
    {
        if (!CompilerOptions::IsLibrary)
        {
            system(("rm " + CompilerOptions::OutputO).c_str());
        }
        if (!CompilerOptions::GenerateLLVMIR)
        {
            system(("rm " + CompilerOptions::OutputLL).c_str());
        }
        system(("rm " + CompilerOptions::OutputS).c_str());
    }

    NumberExprAST::NumberExprAST(double Val) : Val(Val)
    {
    }

    Value *NumberExprAST::codegen()
    {
        return ConstantFP::get(*MasterAST::TheContext, APFloat(Val));
    }

    StringExprAST::StringExprAST(std::string Val) : Val(Val)
    {
    }

    Value *StringExprAST::codegen()
    {
        return MasterAST::Builder->CreateGlobalString(Val + "\n");
    }

    VariableExprAST::VariableExprAST(const std::string &Name)
        : Name(Name)
    {
    }

    Value *VariableExprAST::codegen()
    {
        // Look this variable up in the function.
        Value *V = MasterAST::NamedValues[Name];
        if (!V)
            logError("Unknown variable: '" + Name + "'");
        return V;
    }

    BinaryExprAST::BinaryExprAST(char Op, ExprAST *LHS, ExprAST *RHS)
        : Op(Op), LHS(LHS), RHS(RHS)
    {
    }

    Value *BinaryExprAST::codegen()
    {
        Value *L = LHS->codegen();
        Value *R = RHS->codegen();
        if (!L || !R)
            return nullptr;

        switch (Op)
        {
        case '+':
            return MasterAST::Builder->CreateFAdd(L, R, "addtmp");
        case '-':
            return MasterAST::Builder->CreateFSub(L, R, "subtmp");
        case '*':
            return MasterAST::Builder->CreateFMul(L, R, "multmp");
        case '<':
            L = MasterAST::Builder->CreateFCmpULT(L, R, "cmptmp");
            // Convert bool 0/1 to double 0.0 or 1.0
            return MasterAST::Builder->CreateUIToFP(L, Type::getDoubleTy(*MasterAST::TheContext), "booltmp");
        default:
            return logError("invalid binary operator");
        }
    }

    CallExprAST::CallExprAST(const std::string &Callee, std::vector<ExprAST *> Args)
        : Callee(Callee), Args(Args)
    {
    }

    Value *CallExprAST::codegen()
    {
        // Look up the name in the global module table.
        Function *CalleeF = getFunction(Callee);
        if (!CalleeF)
            return logError("Unknown function referenced");

        // If argument mismatch error.
        if (CalleeF->arg_size() != Args.size())
            return logError("Incorrect # arguments passed");

        std::vector<Value *> ArgsV;
        for (unsigned i = 0, e = Args.size(); i != e; ++i)
        {
            ArgsV.push_back(Args[i]->codegen());
            if (!ArgsV.back())
                return nullptr;
        }

        return MasterAST::Builder->CreateCall(CalleeF, ArgsV, "calltmp");
    }

    PrototypeAST::PrototypeAST(const std::string &Name, std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args))
    {
    }

    const std::string &PrototypeAST::getName() const
    {
        return Name;
    }

    Function *PrototypeAST::codegen()
    {
        // Make the function type:  double(double,double) etc.
        std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*MasterAST::TheContext));

        FunctionType *FT = FunctionType::get(Type::getDoubleTy(*MasterAST::TheContext), Doubles, false);

        Function *F = Function::Create(FT, Function::ExternalLinkage, Name, MasterAST::TheModule);

        // Set names for all arguments.
        unsigned Idx = 0;
        for (auto &Arg : F->args())
        {
            Arg.setName(Args[Idx++]);
        }

        return F;
    }

    FunctionAST::FunctionAST(PrototypeAST *Proto, ExprAST *Body)
        : Proto(Proto), Body(Body)
    {
    }

    Function *FunctionAST::codegen()
    {
        // Transfer ownership of the prototype to the FunctionProtos map, but keep a
        // reference to it for use below.
        auto &P = *Proto;
        MasterAST::FunctionProtos[Proto->getName()] = std::move(Proto);
        Function *TheFunction = getFunction(P.getName());
        if (!TheFunction)
            return nullptr;

        // Create a new basic block to start insertion into.
        BasicBlock *BB = BasicBlock::Create(*MasterAST::TheContext, "entry", TheFunction);
        MasterAST::Builder->SetInsertPoint(BB);

        // Record the function arguments in the NamedValues map.
        MasterAST::NamedValues.clear();
        for (auto &Arg : TheFunction->args())
            MasterAST::NamedValues[std::string(Arg.getName())] = &Arg;

        if (Value *RetVal = Body->codegen())
        {
            // Finish off the function.
            MasterAST::Builder->CreateRet(RetVal);

            // Validate the generated code, checking for consistency.
            verifyFunction(*TheFunction);

            // Run the optimizer on the function.
            MasterAST::TheFPM->run(*TheFunction);

            return TheFunction;
        }

        // Error reading body, remove function.
        TheFunction->eraseFromParent();
        return nullptr;
    }

    void createExternalFunctions()
    {
        auto bytePtrTy = MasterAST::Builder->getInt8Ty()->getPointerTo();

        MasterAST::TheModule->getOrInsertFunction("printf",
                                                  llvm::FunctionType::get(
                                                      /* return type */ MasterAST::Builder->getDoubleTy(),
                                                      /* format arg */ bytePtrTy,
                                                      /* vararg */ true));
    }
}