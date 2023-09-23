#include "ast.h"

namespace AST
{
    void initializeModule()
    {
        TheContext = new LLVMContext;
        TheModule = new Module("my cool jit", *TheContext);

        // Create a new builder for the module.
        Builder = new IRBuilder<>(*TheContext);
    }

    void outputModule()
    {
        std::cout << "----------------------------------------------";
        std::cout << "----------------------------------------------" << std::endl;
        AST::TheModule->print(errs(), nullptr);
    }

    Value *logError(std::string message)
    {
        std::cout << message << std::endl;
        return nullptr;
    }

    NumberExprAST::NumberExprAST(double Val) : Val(Val)
    {
    }

    Value *NumberExprAST::codegen()
    {
        return ConstantFP::get(*TheContext, APFloat(Val));
    }

    VariableExprAST::VariableExprAST(const std::string &Name)
        : Name(Name)
    {
    }

    Value *VariableExprAST::codegen()
    {
        // Look this variable up in the function.
        Value *V = NamedValues[Name];
        if (!V)
            logError("Unknown variable name");
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
            return Builder->CreateFAdd(L, R, "addtmp");
        case '-':
            return Builder->CreateFSub(L, R, "subtmp");
        case '*':
            return Builder->CreateFMul(L, R, "multmp");
        case '<':
            L = Builder->CreateFCmpULT(L, R, "cmptmp");
            // Convert bool 0/1 to double 0.0 or 1.0
            return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
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
        Function *CalleeF = TheModule->getFunction(Callee);
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

        return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
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
        std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*TheContext));

        FunctionType *FT = FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);

        Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule);

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
        // First, check for an existing function from a previous 'extern' declaration.
        Function *TheFunction = TheModule->getFunction(Proto->getName());

        if (!TheFunction)
            TheFunction = Proto->codegen();

        if (!TheFunction)
            return nullptr;

        // Create a new basic block to start insertion into.
        BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
        Builder->SetInsertPoint(BB);

        // Record the function arguments in the NamedValues map.
        NamedValues.clear();
        for (auto &Arg : TheFunction->args())
            NamedValues[std::string(Arg.getName())] = &Arg;

        if (Value *RetVal = Body->codegen())
        {
            // Finish off the function.
            Builder->CreateRet(RetVal);

            // Validate the generated code, checking for consistency.
            verifyFunction(*TheFunction);

            return TheFunction;
        }

        // Error reading body, remove function.
        TheFunction->eraseFromParent();
        return nullptr;
    }
}