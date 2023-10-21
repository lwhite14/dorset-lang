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

    void MasterAST::initializeModule(const char* moduleName)
    {
        TheContext = new LLVMContext;
        TheModule = new Module(moduleName, *TheContext);

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

        // Create a new builder for the module.
        Builder = new IRBuilder<>(*TheContext);
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
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS))
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
        Function *CalleeF = MasterAST::TheModule->getFunction(Callee);
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
        : Proto(std::move(Proto)), Body(std::move(Body))
    {
    }

    Function *FunctionAST::codegen()
    {
        // First, check for an existing function from a previous 'extern' declaration.
        Function *TheFunction = MasterAST::TheModule->getFunction(Proto->getName());

        if (!TheFunction)
            TheFunction = Proto->codegen();

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

            MasterAST::TheFPM->run(*TheFunction);

            return TheFunction;
        }

        // Error reading body, remove function.
        TheFunction->eraseFromParent();
        return nullptr;
    }

    IfExprAST::IfExprAST(ExprAST* Cond, ExprAST* Then, ExprAST* Else)
        : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else))
    {

    }

    Value* IfExprAST::codegen() 
    {
        Value* CondV = Cond->codegen();
        if (!CondV)
            return nullptr;

        // Convert condition to a bool by comparing non-equal to 0.0.
        CondV = MasterAST::Builder->CreateFCmpONE(
            CondV, ConstantFP::get(*MasterAST::TheContext, APFloat(0.0)), "ifcond");

        Function* TheFunction = MasterAST::Builder->GetInsertBlock()->getParent();

        // Create blocks for the then and else cases.  Insert the 'then' block at the
        // end of the function.
        BasicBlock* ThenBB = BasicBlock::Create(*MasterAST::TheContext, "then", TheFunction);
        BasicBlock* ElseBB = BasicBlock::Create(*MasterAST::TheContext, "else");
        BasicBlock* MergeBB = BasicBlock::Create(*MasterAST::TheContext, "ifcont");

        MasterAST::Builder->CreateCondBr(CondV, ThenBB, ElseBB);

        // Emit then value.
        MasterAST::Builder->SetInsertPoint(ThenBB);

        Value* ThenV = Then->codegen();
        if (!ThenV)
            return nullptr;

        MasterAST::Builder->CreateBr(MergeBB);
        // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
        ThenBB = MasterAST::Builder->GetInsertBlock();

        // Emit else block.
        TheFunction->insert(TheFunction->end(), ElseBB);
        MasterAST::Builder->SetInsertPoint(ElseBB);

        Value* ElseV = Else->codegen();
        if (!ElseV)
            return nullptr;

        MasterAST::Builder->CreateBr(MergeBB);
        // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
        ElseBB = MasterAST::Builder->GetInsertBlock();

        // Emit merge block.
        TheFunction->insert(TheFunction->end(), MergeBB);
        MasterAST::Builder->SetInsertPoint(MergeBB);
        PHINode* PN = MasterAST::Builder->CreatePHI(Type::getDoubleTy(*MasterAST::TheContext), 2, "iftmp");

        PN->addIncoming(ThenV, ThenBB);
        PN->addIncoming(ElseV, ElseBB);
        return PN;
    }

    ForExprAST::ForExprAST(const std::string& VarName, ExprAST* Start, ExprAST* End, ExprAST* Step, ExprAST* Body)
        : VarName(VarName), Start(std::move(Start)), End(std::move(End)),
        Step(std::move(Step)), Body(std::move(Body)) 
    {
    }

    Value* ForExprAST::codegen() 
    {
        // Emit the start code first, without 'variable' in scope.
        Value* StartVal = Start->codegen();
        if (!StartVal)
            return nullptr;

        // Make the new basic block for the loop header, inserting after current
        // block.
        Function* TheFunction = MasterAST::Builder->GetInsertBlock()->getParent();
        BasicBlock* PreheaderBB = MasterAST::Builder->GetInsertBlock();
        BasicBlock* LoopBB = BasicBlock::Create(*MasterAST::TheContext, "loop", TheFunction);

        // Insert an explicit fall through from the current block to the LoopBB.
        MasterAST::Builder->CreateBr(LoopBB);

        // Start insertion in LoopBB.
        MasterAST::Builder->SetInsertPoint(LoopBB);

        // Start the PHI node with an entry for Start.
        PHINode* Variable =
            MasterAST::Builder->CreatePHI(Type::getDoubleTy(*MasterAST::TheContext), 2, VarName);
        Variable->addIncoming(StartVal, PreheaderBB);

        // Within the loop, the variable is defined equal to the PHI node.  If it
        // shadows an existing variable, we have to restore it, so save it now.
        Value* OldVal = MasterAST::NamedValues[VarName];
        MasterAST::NamedValues[VarName] = Variable;

        // Emit the body of the loop.  This, like any other expr, can change the
        // current BB.  Note that we ignore the value computed by the body, but don't
        // allow an error.
        if (!Body->codegen())
            return nullptr;

        // Emit the step value.
        Value* StepVal = nullptr;
        if (Step) {
            StepVal = Step->codegen();
            if (!StepVal)
                return nullptr;
        }
        else {
            // If not specified, use 1.0.
            StepVal = ConstantFP::get(*MasterAST::TheContext, APFloat(1.0));
        }

        Value* NextVar = MasterAST::Builder->CreateFAdd(Variable, StepVal, "nextvar");

        // Compute the end condition.
        Value* EndCond = End->codegen();
        if (!EndCond)
            return nullptr;

        // Convert condition to a bool by comparing non-equal to 0.0.
        EndCond = MasterAST::Builder->CreateFCmpONE(
            EndCond, ConstantFP::get(*MasterAST::TheContext, APFloat(0.0)), "loopcond");

        // Create the "after loop" block and insert it.
        BasicBlock* LoopEndBB = MasterAST::Builder->GetInsertBlock();
        BasicBlock* AfterBB =
            BasicBlock::Create(*MasterAST::TheContext, "afterloop", TheFunction);

        // Insert the conditional branch into the end of LoopEndBB.
        MasterAST::Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

        // Any new code will be inserted in AfterBB.
        MasterAST::Builder->SetInsertPoint(AfterBB);

        // Add a new entry to the PHI node for the backedge.
        Variable->addIncoming(NextVar, LoopEndBB);

        // Restore the unshadowed variable.
        if (OldVal)
            MasterAST::NamedValues[VarName] = OldVal;
        else
            MasterAST::NamedValues.erase(VarName);

        // for expr always returns 0.0.
        return Constant::getNullValue(Type::getDoubleTy(*MasterAST::TheContext));
    }

    void createExternalFunctions()
    {
        auto bytePtrTy = MasterAST::Builder->getInt8Ty()->getPointerTo();
        auto doubleTy = MasterAST::Builder->getDoubleTy();

        MasterAST::TheModule->getOrInsertFunction("printf",
                                       llvm::FunctionType::get(
                                           /* return type */ MasterAST::Builder->getDoubleTy(),
                                           /* format arg */ { bytePtrTy, doubleTy },
                                           /* vararg */ true));
    }
}