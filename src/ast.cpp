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
        auto FI = MasterAST::FunctionProtos.find(Name);
        if (FI != MasterAST::FunctionProtos.end())
            return FI->second->codegen();

        // If no existing prototype exists, return null.
        return nullptr;
    }

    AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName)
    {
        IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
        return TmpB.CreateAlloca(Type::getDoubleTy(*MasterAST::TheContext), nullptr, VarName);
    }

    void MasterAST::initializeModule(const char *moduleName)
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
        if (Val == "\\n")
        {
            std::string val = "";
            return MasterAST::Builder->CreateGlobalString(val + "\n");
        }

        return MasterAST::Builder->CreateGlobalString(Val);
    }

    VariableExprAST::VariableExprAST(const std::string &Name)
        : Name(Name)
    {
    }

    Value *VariableExprAST::codegen()
    {
        // Look this variable up in the function.
        AllocaInst *A = MasterAST::NamedValues[Name];
        if (!A)
            return logError("unknown variable name: " + Name);

        // Load the value.
        return MasterAST::Builder->CreateLoad(A->getAllocatedType(), A, Name.c_str());
    }

    const std::string &VariableExprAST::getName()
    {
        return Name;
    }

    VarExprAST::VarExprAST(std::string Name, ExprAST* Init)
        : Name(Name), Init(std::move(Init))
    {
    }

    Value *VarExprAST::codegen()
    {
        Function *TheFunction = MasterAST::Builder->GetInsertBlock()->getParent();

        Value *InitVal;
        if (Init != nullptr)
        {
            InitVal = Init->codegen();
            if (InitVal == nullptr)
            { 
                ErrorHandler::error("variable initialization has failed");
                return nullptr;
            }

        }
        else
        { // If not specified, use 0.0.
            InitVal = ConstantFP::get(*MasterAST::TheContext, APFloat(0.0));
        }


        AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, Name);
        MasterAST::Builder->CreateStore(InitVal, Alloca);

        // Remember this binding.
        MasterAST::NamedValues[Name] = Alloca;

        // Return the body computation.
        return InitVal;
    }

    BinaryExprAST::BinaryExprAST(char Op, ExprAST *LHS, ExprAST *RHS)
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS))
    {
    }

    Value *BinaryExprAST::codegen()
    {
        if (Op == '=')
        {
            VariableExprAST *LHSE = static_cast<VariableExprAST *>(LHS);
            if (!LHSE)
                return logError("destination of '=' must be a variable");
            // Codegen the RHS.
            Value *Val = RHS->codegen();
            if (!Val)
                return nullptr;

            // Look up the name.
            Value *Variable = MasterAST::NamedValues[LHSE->getName()];
            if (!Variable)
                return logError("unknown variable name: " + LHSE->getName());

            MasterAST::Builder->CreateStore(Val, Variable);
            return Val;
        }

        Value *L = LHS->codegen();
        Value *R = RHS->codegen();
        if (!L || !R)
            return nullptr;

        if (Op == '+')
        {
            return MasterAST::Builder->CreateFAdd(L, R, "addtmp");
        }
        else if (Op == '-')
        {
            return MasterAST::Builder->CreateFSub(L, R, "subtmp");
        }
        else if (Op == '*')
        {
            return MasterAST::Builder->CreateFMul(L, R, "multmp");
        }
        else if (Op == '<')
        {
            L = MasterAST::Builder->CreateFCmpULT(L, R, "cmptmp");
            // Convert bool 0/1 to double 0.0 or 1.0
            return MasterAST::Builder->CreateUIToFP(L, Type::getDoubleTy(*MasterAST::TheContext), "booltmp");
        }

        // If it wasn't a builtin binary operator, it must be a user defined one. Emit
        // a call to it.
        Function *F = getFunction(std::string("binary") + Op);
        assert(F && "binary operator not found!");

        Value *Ops[] = {L, R};
        return MasterAST::Builder->CreateCall(F, Ops, "binop");
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
            return logError("unknown function referenced: " + Callee);

        // If argument mismatch error.
        if (CalleeF->arg_size() != Args.size())
            return logError("incorrect # arguments passed");

        std::vector<Value *> ArgsV;
        for (unsigned i = 0, e = Args.size(); i != e; ++i)
        {
            ArgsV.push_back(Args[i]->codegen());
            if (!ArgsV.back())
                return nullptr;
        }

        return MasterAST::Builder->CreateCall(CalleeF, ArgsV, "calltmp");
    }

    PrototypeAST::PrototypeAST(const std::string &Name, std::vector<std::string> Args, bool IsOperator, unsigned Prec)
        : Name(Name), Args(std::move(Args)), IsOperator(IsOperator), Precedence(Prec)
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

    bool PrototypeAST::isUnaryOp() const
    {
        return IsOperator && Args.size() == 1;
    }

    bool PrototypeAST::isBinaryOp() const
    {
        return IsOperator && Args.size() == 2;
    }

    char PrototypeAST::getOperatorName() const
    {
        assert(isUnaryOp() || isBinaryOp());
        return Name[Name.size() - 1];
    }

    unsigned PrototypeAST::getBinaryPrecedence() const
    {
        return Precedence;
    }

    FunctionAST::FunctionAST(PrototypeAST *Proto, ExprAST *Body)
        : Proto(std::move(Proto)), Body(std::move(Body))
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

        // If this is an operator, install it.
        if (P.isBinaryOp())
            MasterAST::BinopPrecedence[P.getOperatorName()] = P.getBinaryPrecedence();

        // Create a new basic block to start insertion into.
        BasicBlock *BB = BasicBlock::Create(*MasterAST::TheContext, "entry", TheFunction);
        MasterAST::Builder->SetInsertPoint(BB);

        // Record the function arguments in the NamedValues map.
        MasterAST::NamedValues.clear();
        for (auto &Arg : TheFunction->args())
        {
            // Create an alloca for this variable.
            AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName().str());

            // Store the initial value into the alloca.
            MasterAST::Builder->CreateStore(&Arg, Alloca);

            // Add arguments to variable symbol table.
            MasterAST::NamedValues[std::string(Arg.getName())] = Alloca;
        }

        if (Value *RetVal = Body->codegen())
        {
            // Finish off the function.
            MasterAST::Builder->CreateRet(RetVal);

            // Validate the generated code, checking for consistency.
            verifyFunction(*TheFunction);

            return TheFunction;
        }

        // Error reading body, remove function.
        TheFunction->eraseFromParent();

        if (P.isBinaryOp())
            MasterAST::BinopPrecedence.erase(P.getOperatorName());
        return nullptr;
    }

    IfExprAST::IfExprAST(ExprAST *Cond, ExprAST *Then, ExprAST *Else)
        : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else))
    {
    }

    Value *IfExprAST::codegen()
    {
        Value *CondV = Cond->codegen();
        if (!CondV)
            return nullptr;

        // Convert condition to a bool by comparing non-equal to 0.0.
        CondV = MasterAST::Builder->CreateFCmpONE(
            CondV, ConstantFP::get(*MasterAST::TheContext, APFloat(0.0)), "ifcond");

        Function *TheFunction = MasterAST::Builder->GetInsertBlock()->getParent();

        // Create blocks for the then and else cases.  Insert the 'then' block at the
        // end of the function.
        BasicBlock *ThenBB = BasicBlock::Create(*MasterAST::TheContext, "then", TheFunction);
        BasicBlock *ElseBB = BasicBlock::Create(*MasterAST::TheContext, "else");
        BasicBlock *MergeBB = BasicBlock::Create(*MasterAST::TheContext, "ifcont");

        MasterAST::Builder->CreateCondBr(CondV, ThenBB, ElseBB);

        // Emit then value.
        MasterAST::Builder->SetInsertPoint(ThenBB);

        Value *ThenV = Then->codegen();
        if (!ThenV)
            return nullptr;

        MasterAST::Builder->CreateBr(MergeBB);
        // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
        ThenBB = MasterAST::Builder->GetInsertBlock();

        // Emit else block.
        TheFunction->insert(TheFunction->end(), ElseBB);
        MasterAST::Builder->SetInsertPoint(ElseBB);

        Value *ElseV = Else->codegen();
        if (!ElseV)
            return nullptr;

        MasterAST::Builder->CreateBr(MergeBB);
        // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
        ElseBB = MasterAST::Builder->GetInsertBlock();

        // Emit merge block.
        TheFunction->insert(TheFunction->end(), MergeBB);
        MasterAST::Builder->SetInsertPoint(MergeBB);
        PHINode *PN = MasterAST::Builder->CreatePHI(Type::getDoubleTy(*MasterAST::TheContext), 2, "iftmp");

        PN->addIncoming(ThenV, ThenBB);
        PN->addIncoming(ElseV, ElseBB);
        return PN;
    }

    ForExprAST::ForExprAST(const std::string &VarName, ExprAST *Start, ExprAST *End, ExprAST *Step, ExprAST *Body)
        : VarName(VarName), Start(std::move(Start)), End(std::move(End)),
          Step(std::move(Step)), Body(std::move(Body))
    {
    }

    Value *ForExprAST::codegen()
    {
        Function *TheFunction = MasterAST::Builder->GetInsertBlock()->getParent();

        // Create an alloca for the variable in the entry block.
        AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, VarName);

        // Emit the start code first, without 'variable' in scope.
        Value *StartVal = Start->codegen();
        if (!StartVal)
            return nullptr;

        // Store the value into the alloca.
        MasterAST::Builder->CreateStore(StartVal, Alloca);

        // Make the new basic block for the loop header, inserting after current
        // block.
        BasicBlock *LoopBB = BasicBlock::Create(*MasterAST::TheContext, "loop", TheFunction);

        // Insert an explicit fall through from the current block to the LoopBB.
        MasterAST::Builder->CreateBr(LoopBB);

        // Start insertion in LoopBB.
        MasterAST::Builder->SetInsertPoint(LoopBB);

        // Within the loop, the variable is defined equal to the PHI node.  If it
        // shadows an existing variable, we have to restore it, so save it now.
        AllocaInst *OldVal = MasterAST::NamedValues[VarName];
        MasterAST::NamedValues[VarName] = Alloca;

        // Emit the body of the loop.  This, like any other expr, can change the
        // current BB.  Note that we ignore the value computed by the body, but don't
        // allow an error.
        if (!Body->codegen())
            return nullptr;

        // Emit the step value.
        Value *StepVal = nullptr;
        if (Step)
        {
            StepVal = Step->codegen();
            if (!StepVal)
                return nullptr;
        }
        else
        {
            // If not specified, use 1.0.
            StepVal = ConstantFP::get(*MasterAST::TheContext, APFloat(1.0));
        }

        // Compute the end condition.
        Value *EndCond = End->codegen();
        if (!EndCond)
            return nullptr;

        // Reload, increment, and restore the alloca.  This handles the case where
        // the body of the loop mutates the variable.
        Value *CurVar =
            MasterAST::Builder->CreateLoad(Alloca->getAllocatedType(), Alloca, VarName.c_str());
        Value *NextVar = MasterAST::Builder->CreateFAdd(CurVar, StepVal, "nextvar");
        MasterAST::Builder->CreateStore(NextVar, Alloca);

        // Convert condition to a bool by comparing non-equal to 0.0.
        EndCond = MasterAST::Builder->CreateFCmpONE(
            EndCond, ConstantFP::get(*MasterAST::TheContext, APFloat(0.0)), "loopcond");

        // Create the "after loop" block and insert it.
        BasicBlock *AfterBB =
            BasicBlock::Create(*MasterAST::TheContext, "afterloop", TheFunction);

        // Insert the conditional branch into the end of LoopEndBB.
        MasterAST::Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

        // Any new code will be inserted in AfterBB.
        MasterAST::Builder->SetInsertPoint(AfterBB);

        // Restore the unshadowed variable.
        if (OldVal)
            MasterAST::NamedValues[VarName] = OldVal;
        else
            MasterAST::NamedValues.erase(VarName);

        // for expr always returns 0.0.
        return Constant::getNullValue(Type::getDoubleTy(*MasterAST::TheContext));
    }

    UnaryExprAST::UnaryExprAST(char Opcode, ExprAST *Operand)
        : Opcode(Opcode), Operand(std::move(Operand))
    {
    }

    Value *UnaryExprAST::codegen()
    {
        Value *OperandV = Operand->codegen();
        if (!OperandV)
            return nullptr;

        Function *F = getFunction(std::string("unary") + Opcode);
        if (!F)
            return logError("unknown unary operator");

        return MasterAST::Builder->CreateCall(F, OperandV, "unop");
    }

    void createExternalFunctions()
    {
        auto bytePtrTy = MasterAST::Builder->getInt8Ty()->getPointerTo();
        auto doubleTy = MasterAST::Builder->getDoubleTy();

        MasterAST::TheModule->getOrInsertFunction("printf",
                                                  llvm::FunctionType::get(
                                                      /* return type */ MasterAST::Builder->getDoubleTy(),
                                                      /* format arg */ {bytePtrTy, doubleTy},
                                                      /* vararg */ true));
    }
}