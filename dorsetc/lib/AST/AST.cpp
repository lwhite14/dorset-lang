#include <dorset-lang/AST/AST.h>

#include <fstream>
#include <ostream>

namespace Dorset
{
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

        AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type* type)
        {
            IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
            return TmpB.CreateAlloca(type, nullptr, VarName);
            // return TmpB.CreateAlloca(Type::getDoubleTy(*MasterAST::TheContext), nullptr, VarName);
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
                    return logError("variable initialization has failed");
                }

            }
            else
            { // If not specified, use 0.0.
                InitVal = ConstantFP::get(*MasterAST::TheContext, APFloat(0.0));
            }


            AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, Name, Type::getDoubleTy(*MasterAST::TheContext));
            MasterAST::Builder->CreateStore(InitVal, Alloca);

            // Remember this binding.
            MasterAST::NamedValues[Name] = Alloca;

            // Return the body computation.
            return InitVal;
        }

        ArrayExprAST::ArrayExprAST(std::string Name, int Size, std::vector<ExprAST*> Values)
            : Name(Name), Size(Size), Values(Values)
        {       
        }

        Value* ArrayExprAST::codegen()
        {
            // Create the global array variable
            ArrayType *ArrayType = ArrayType::get(Type::getDoubleTy(*MasterAST::TheContext), Size);
            Array = new GlobalVariable(*MasterAST::TheModule, ArrayType, false, GlobalValue::CommonLinkage, nullptr, Name);

            Constant *ArrayValues[Size];

            for (unsigned int i = 0; i < Size; i++)
            {
                ArrayValues[i] = ConstantFP::get(*MasterAST::TheContext, APFloat(0.0));
            }

            Array->setInitializer(ConstantArray::get(ArrayType, *ArrayValues));


            for (unsigned int i = 0; i < Size; i++)
            {
                // Specify the indices for the element you want to assign
                Value *indices[] = {
                    ConstantInt::get(*MasterAST::TheContext, APInt(32, 0)),
                    ConstantInt::get(*MasterAST::TheContext, APInt(32, i))
                };

                Value *ElementPtr = MasterAST::Builder->CreateGEP(ArrayType, Array, indices, Name + std::to_string(i));
                MasterAST::Builder->CreateStore(Values[i]->codegen(), ElementPtr);
            }

            MasterAST::Arrays[Name] = this;

            return Array;
        }

        const int ArrayExprAST::getSize()
        {
            return Size;
        }

        GlobalVariable *ArrayExprAST::getArray()
        {
            return Array;
        }

        ArrayElementRefExprAST::ArrayElementRefExprAST(const std::string &ArrayName, int Index)
            : ArrayName(ArrayName), Index(Index)
        {
        }

        Value *ArrayElementRefExprAST::codegen()
        {
            ArrayExprAST *WorkingArray = MasterAST::Arrays[ArrayName];

            ArrayType *ArrayType = ArrayType::get(Type::getDoubleTy(*MasterAST::TheContext), WorkingArray->getSize());

            Value *indices[] = {
                ConstantInt::get(*MasterAST::TheContext, APInt(32, 0)),
                ConstantInt::get(*MasterAST::TheContext, APInt(32, Index))
            };

            Value *elementPtr = MasterAST::Builder->CreateGEP(ArrayType, WorkingArray->getArray(), indices);
            Value *loadedValue = MasterAST::Builder->CreateLoad(Type::getDoubleTy(*MasterAST::TheContext), elementPtr);

            return loadedValue;
        }

        const std::string& ArrayElementRefExprAST::getName()
        {
            return ArrayName;
        }

        const int ArrayElementRefExprAST::getIndex()
        {
            return Index;
        }

        BinaryExprAST::BinaryExprAST(char Op, ExprAST *LHS, ExprAST *RHS)
            : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS))
        {
        }

        Value *BinaryExprAST::codegen()
        {
            if (Op == '=')
            {
                bool isArray = false;

                VariableExprAST         *LHS_Variable = dynamic_cast<VariableExprAST *>         (LHS);
                ArrayElementRefExprAST  *LHS_ArrayRef = dynamic_cast<ArrayElementRefExprAST *>  (LHS);

                if (LHS_Variable == nullptr)
                {
                    isArray = true;
                }

                if (isArray)
                {
                    if (!LHS_ArrayRef)
                    {
                        return logError("destination of '=' must be a variable");
                    }

                    // Codegen the RHS.
                    Value *Val = RHS->codegen();
                    if (!Val)
                    {
                        return logError("'right hand side' generation failed for variable: " + LHS_ArrayRef->getName());
                    }

                    // Look up the name.
                    ArrayExprAST *WorkingArray = MasterAST::Arrays[LHS_ArrayRef->getName()];
                    if (!WorkingArray)
                    {
                        return logError("unknown array name: " + LHS_ArrayRef->getName());
                    }

                    ArrayType *ArrayType = ArrayType::get(Type::getDoubleTy(*MasterAST::TheContext), WorkingArray->getSize());

                    Value *indices[] = {
                        ConstantInt::get(*MasterAST::TheContext, APInt(32, 0)),
                        ConstantInt::get(*MasterAST::TheContext, APInt(32, LHS_ArrayRef->getIndex()))
                    };

                    Value *ElementPtr = MasterAST::Builder->CreateGEP(ArrayType, WorkingArray->getArray(), indices, LHS_ArrayRef->getName() + std::to_string(LHS_ArrayRef->getIndex()));
                    MasterAST::Builder->CreateStore(Val, ElementPtr);

                    return Val;
                }
                else
                {
                    if (!LHS_Variable)
                    {
                        return logError("destination of '=' must be a variable");
                    }

                    // Codegen the RHS.
                    Value *Val = RHS->codegen();
                    if (!Val)
                    {
                        return logError("'right hand side' generation failed for variable: " + LHS_Variable->getName());
                    }

                    // Look up the name.
                    Value *Variable = MasterAST::NamedValues[LHS_Variable->getName()];

                    if (!Variable)
                    {
                        return logError("unknown variable name: " + LHS_ArrayRef->getName());
                    }

                    MasterAST::Builder->CreateStore(Val, Variable);
                    return Val;
                }
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

            if (CalleeF->getReturnType() == Type::getVoidTy(*MasterAST::TheContext))
                return MasterAST::Builder->CreateCall(CalleeF, ArgsV, "");
            else 
                return MasterAST::Builder->CreateCall(CalleeF, ArgsV, "calltmp");
        }

        PrototypeArgumentAST::PrototypeArgumentAST(std::string Name, std::string ArgType)
            : Name(Name)
        {
            if (ArgType == "double")
            {
                this->ArgType = MasterAST::Builder->getDoubleTy();
            }
            else if (ArgType == "string")
            {
                this->ArgType = MasterAST::Builder->getInt8Ty()->getPointerTo();
            }
            else
            {
                ErrorHandler::warning("could not parse argument type: " + Name + ", double assumed");
                this->ArgType = MasterAST::Builder->getDoubleTy(); 
            }
        }

        std::string PrototypeArgumentAST::getName()
        {
            return Name;
        }

        Type* PrototypeArgumentAST::getType()
        {
            return ArgType;
        }

        PrototypeAST::PrototypeAST(const std::string& Name, std::vector<PrototypeArgumentAST*> Args, std::string ReturnType, bool IsOperator, unsigned Prec)
            : Name(Name), Args(std::move(Args)), IsOperator(IsOperator), Precedence(Prec), ReturnType(ReturnType)
        {
        }

        const std::string &PrototypeAST::getName() const
        {
            return Name;
        }

        const std::string &PrototypeAST::getReturnType() const
        {
            return ReturnType;
        }

        Function *PrototypeAST::codegen()
        {
            // Make the function type:  double(double,double) etc.
            // std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*MasterAST::TheContext));

            std::vector<Type*> ArgsTypes;
            for (unsigned int i = 0; i < Args.size(); i++)
            {
                ArgsTypes.push_back(Args[i]->getType());
            }

            llvm::Type* type;

            if (ReturnType == "void") 
            {
                type = Type::getVoidTy(*MasterAST::TheContext);
            }
            else if (ReturnType == "double")
            {
                type = Type::getDoubleTy(*MasterAST::TheContext);
            }
            else
            {
                type = Type::getVoidTy(*MasterAST::TheContext);
            }

            FunctionType* FT = FunctionType::get(type, ArgsTypes, false);
            Function* F = Function::Create(FT, Function::ExternalLinkage, Name, MasterAST::TheModule);

            // Set names for all arguments.
            unsigned Idx = 0;
            for (auto &Arg : F->args())
            {
                Arg.setName(Args[Idx++]->getName());
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

        FunctionAST::FunctionAST(PrototypeAST *Proto, BlockAST *Body)
            : Proto(std::move(Proto)), Body(Body)
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
            MasterAST::Arrays.clear();
            for (auto &Arg : TheFunction->args())
            {
                // Create an alloca for this variable.
                AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, Arg.getName().str(), Type::getDoubleTy(*MasterAST::TheContext));

                // Store the initial value into the alloca.
                MasterAST::Builder->CreateStore(&Arg, Alloca);

                // Add arguments to variable symbol table.
                MasterAST::NamedValues[std::string(Arg.getName())] = Alloca;
            }

            Body->codegen();

            if (P.getReturnType() == "void") 
            {
                MasterAST::Builder->CreateRet(nullptr);
            }

            // Validate the generated code, checking for consistency.
            verifyFunction(*TheFunction);

            return TheFunction;
        }

        IfExprAST::IfExprAST(ExprAST *Cond, ExprAST *Then, ExprAST *Else, bool ThenReturns, bool ElseReturns)
            : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)), ThenReturns(ThenReturns), ElseReturns(ElseReturns)
        {
        }

        Value *IfExprAST::codegen()
        {
            bool NeedsIfCont = true;
            if (ThenReturns && ElseReturns)
            {
                NeedsIfCont = false;
            }

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

            if (!ThenReturns)
            {
                MasterAST::Builder->CreateBr(MergeBB);
            }
            // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
            ThenBB = MasterAST::Builder->GetInsertBlock();

            // Emit else block.
            TheFunction->insert(TheFunction->end(), ElseBB);
            MasterAST::Builder->SetInsertPoint(ElseBB);

            if (Else)
            {
                Value *ElseV = Else->codegen();
                if (!ElseV)
                    return nullptr;
            }

            if (!ElseReturns)
            {
                MasterAST::Builder->CreateBr(MergeBB);
            }
            // Codegen of 'Else' can change the current block, update ElseBB for the PHI.
            ElseBB = MasterAST::Builder->GetInsertBlock();

            // Emit merge block.
            if (NeedsIfCont)
            {
                TheFunction->insert(TheFunction->end(), MergeBB);
                MasterAST::Builder->SetInsertPoint(MergeBB);
            }

            // if expr always returns 0.0.
            return Constant::getNullValue(Type::getDoubleTy(*MasterAST::TheContext));
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
            AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, VarName, Type::getDoubleTy(*MasterAST::TheContext));

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

        ReturnExprAST::ReturnExprAST(ExprAST* Expr)
            : Expr(Expr)
        {
        
        }

        Value* ReturnExprAST::codegen()
        {
            Value* RetVal = nullptr;

            if (Expr != nullptr) 
            {
                RetVal = Expr->codegen();
                if (RetVal == nullptr)
                {
                    return logError("return value failed");
                }
            }

            MasterAST::Builder->CreateRet(RetVal);
            return RetVal;
        }

        BlockAST::BlockAST(std::vector<ExprAST*> Exprs) 
            : Exprs(Exprs)
        {
        }

        Value *BlockAST::codegen()
        {
            for (unsigned int i = 0; i < Exprs.size(); i++)
            {
                if (!Exprs[i]->codegen())
                {
                    return logError("expression in block failed");
                }
            }

            return Constant::getNullValue(Type::getDoubleTy(*MasterAST::TheContext));
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


            createPrintFunction();
            createNewLineFunction();
        }

        void createNewLineFunction()
        {
            PrototypeAST* proto = new PrototypeAST("newLine", std::vector<PrototypeArgumentAST*>(), "void");
            std::vector<ExprAST*> args;
            args.push_back(new StringExprAST("\n"));
            args.push_back(new NumberExprAST(0));
            std::vector<ExprAST*> exprs;
            exprs.push_back(new AST::CallExprAST("printf", std::move(args)));
            BlockAST* block = new BlockAST(exprs);
            FunctionAST* function = new FunctionAST(proto, block);

            function->codegen();
        }

        void createPrintFunction()
        {
            std::vector<PrototypeArgumentAST*> protoArgs;
            protoArgs.push_back(new PrototypeArgumentAST("STR", "string"));

            PrototypeAST* proto = new PrototypeAST("print", protoArgs, "void");

            auto& P = *proto;
            MasterAST::FunctionProtos[proto->getName()] = std::move(proto);
            Function* TheFunction = getFunction(P.getName());

            BasicBlock* BB = BasicBlock::Create(*MasterAST::TheContext, "entry", TheFunction);
            MasterAST::Builder->SetInsertPoint(BB);

            MasterAST::NamedValues.clear();

            Value* stringVal = TheFunction->args().begin();

            Function* CalleeF = getFunction("printf");

            std::vector<Value*> ArgsV;
            ArgsV.push_back(stringVal);
            double val = 0;
            ArgsV.push_back(ConstantFP::get(*MasterAST::TheContext, APFloat(val)));

            MasterAST::Builder->CreateCall(CalleeF, ArgsV, "calltmp");      

            MasterAST::Builder->CreateRet(nullptr);

            verifyFunction(*TheFunction);
        }
    }
}