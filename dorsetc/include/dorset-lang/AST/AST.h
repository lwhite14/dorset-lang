#pragma once

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <map>

#include <dorset-lang/Utils/Error.h>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Linker/Linker.h>

using namespace llvm;

namespace Dorset
{
    namespace AST
    {
        Value *logError(std::string message);
        Function *getFunction(std::string Name);
        AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, Type* type);
    

        /// ExprAST - Base class for all expression nodes.
        class ExprAST
        {
        public:
            virtual ~ExprAST() = default;
            virtual Value *codegen() = 0;
        };

        /// NumberExprAST - Expression class for numeric literals like "1.0".
        class NumberExprAST : public ExprAST
        {
            double Val;

        public:
            NumberExprAST(double Val);
            Value *codegen() override;
        };

        class StringExprAST : public ExprAST
        {
            std::string Val;

        public:
            StringExprAST(std::string Val);
            Value *codegen() override;
        };

        /// VariableExprAST - Expression class variable references 'x = 4'
        class VariableExprAST : public ExprAST
        {
            std::string Name;

        public:
            VariableExprAST(const std::string &Name);
            Value *codegen() override;
            const std::string& getName();
        };

        /// VarExprAST - Expression class for variables 'var x = 3'
        class VarExprAST : public ExprAST 
        {
            std::string Name;
            ExprAST* Init;

        public:
            VarExprAST(std::string Name, ExprAST* Init);

            Value* codegen() override;
        };


        // ArrayExprAST - Expression class for double arrays
        class ArrayExprAST : public ExprAST 
        {
            std::string Name;
            int Size;
            std::vector<ExprAST*> Values;
            GlobalVariable *Array;

        public:
            ArrayExprAST(std::string Name, int Size, std::vector<ExprAST*> Values);

            Value* codegen() override;

            const int getSize();
            GlobalVariable *getArray();

        };

        class ArrayElementRefExprAST : public ExprAST
        {
            std::string ArrayName;
            ExprAST *Index;

        public:
            ArrayElementRefExprAST(const std::string &ArrayName, ExprAST *Index);
            Value *codegen() override;
            const std::string& getName();
            Value *getIndex();
        };

        /// BinaryExprAST - Expression class for a binary operator.
        class BinaryExprAST : public ExprAST
        {
            char Op;
            ExprAST *LHS;
            ExprAST *RHS;

        public:
            BinaryExprAST(char Op, ExprAST *LHS, ExprAST *RHS);
            Value *codegen() override;
        };

        /// CallExprAST - Expression class for function calls.
        class CallExprAST : public ExprAST
        {
            std::string Callee;
            std::vector<ExprAST *> Args;

        public:
            CallExprAST(const std::string &Callee, std::vector<ExprAST *> Args);
            Value *codegen() override;
        };

        /// BlockAST - Represents a block, '{ }'.
        class BlockAST : public ExprAST 
        { 
            std::vector<ExprAST*> Exprs;

        public:
            BlockAST(std::vector<ExprAST*> Exprs);

            Value *codegen() override;
        };

        class PrototypeArgumentAST 
        {
            std::string Name;
            Type* ArgType;

        public:
            PrototypeArgumentAST(std::string Name, std::string ArgType);

            std::string getName();
            Type* getType();
        };

        /// PrototypeAST - This class represents the "prototype" for a function,
        /// which captures its argument names as well as if it is an operator.
        class PrototypeAST 
        {
            std::string Name;
            std::vector<PrototypeArgumentAST*> Args;
            bool IsOperator;
            unsigned Precedence;
            std::string ReturnType;

        public:
            PrototypeAST(const std::string& Name, std::vector<PrototypeArgumentAST*> Args, std::string ReturnType, bool IsOperator = false, unsigned Prec = 0);

            Function* codegen();
            const std::string& getName() const;
            const std::string& getReturnType() const;

            bool isUnaryOp() const;
            bool isBinaryOp() const;

            char getOperatorName() const;

            unsigned getBinaryPrecedence() const;
        };

        /// FunctionAST - This class represents a function definition itself.
        class FunctionAST
        {
            PrototypeAST *Proto;
            BlockAST* Body;

        public:
            FunctionAST(PrototypeAST *Proto, BlockAST *Body);
            Function *codegen();
        };

        /// IfExprAST - Expression class for if/then/else.
        class IfExprAST : public ExprAST 
        {
            ExprAST* Cond;
            ExprAST* Then;
            ExprAST* Else;

            bool ThenReturns = false;
            bool ElseReturns = false;

        public:
            IfExprAST(ExprAST* Cond, ExprAST* Then, ExprAST* Else, bool ThenReturns, bool ElseReturns);

            Value* codegen() override;
        };

        /// ForExprAST - Expression class for for/in.
        class ForExprAST : public ExprAST
        {
            std::string VarName;
            ExprAST* Start; 
            ExprAST* End; 
            ExprAST* Step; 
            ExprAST* Body;

        public:
            ForExprAST(const std::string& VarName, ExprAST* Start, ExprAST* End, ExprAST* Step, ExprAST* Body);

            Value* codegen() override;
        };

        /// UnaryExprAST - Expression class for a unary operator.
        class UnaryExprAST : public ExprAST 
        {
            char Opcode;
            ExprAST* Operand;

        public:
            UnaryExprAST(char Opcode, ExprAST* Operand);

            Value* codegen() override;
        };

        /// ReturnExprAST - Expression that represents the return value of a function.
        class ReturnExprAST : public ExprAST 
        {
            ExprAST* Expr;

        public:
            ReturnExprAST(ExprAST* Expr);
            
            Value* codegen() override;
        };

        class MasterAST
        {
        public:
            static inline LLVMContext* TheContext;
            static inline Module* TheModule;
            static inline IRBuilder<>* Builder;
            static inline std::map<std::string, AllocaInst*> NamedValues;
            static inline std::map<std::string, ArrayExprAST*> Arrays;
            static inline legacy::FunctionPassManager* TheFPM;
            static inline std::map<std::string, PrototypeAST*> FunctionProtos;
            static inline std::map<char, int> BinopPrecedence =
            {
                {'=' , 2},
                {'<', 10},
                {'+', 20},
                {'-', 30},
                {'*', 40}
            };

            static void initializeModule(const char* moduleName);
        };

        void createExternalFunctions();
        void createNewLineFunction();
        void createPrintFunction();
    }
}