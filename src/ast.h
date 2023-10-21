#pragma once

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <map>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

using namespace llvm;

namespace AST
{
    Value *logError(std::string message);
    
    class MasterAST
    {
    public:
        static inline LLVMContext *TheContext;
        static inline Module *TheModule;
        static inline IRBuilder<> *Builder;
        static inline std::map<std::string, Value *> NamedValues;
        static inline legacy::FunctionPassManager *TheFPM;

        static void initializeModule(const char* moduleName);
    };

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
    private:
        double Val;

    public:
        NumberExprAST(double Val);
        Value *codegen() override;
    };

    class StringExprAST : public ExprAST
    {
    private:
        std::string Val;

    public:
        StringExprAST(std::string Val);
        Value *codegen() override;
    };

    /// VariableExprAST - Expression class for referencing a variable, like "a".
    class VariableExprAST : public ExprAST
    {
    private:
        std::string Name;

    public:
        VariableExprAST(const std::string &Name);
        Value *codegen() override;
    };

    /// BinaryExprAST - Expression class for a binary operator.
    class BinaryExprAST : public ExprAST
    {
    private:
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
    private:
        std::string Callee;
        std::vector<ExprAST *> Args;

    public:
        CallExprAST(const std::string &Callee, std::vector<ExprAST *> Args);
        Value *codegen() override;
    };

    /// PrototypeAST - This class represents the "prototype" for a function,
    /// which captures its name, and its argument names (thus implicitly the number
    /// of arguments the function takes).
    class PrototypeAST
    {
        std::string Name;
        std::vector<std::string> Args;

    public:
        PrototypeAST(const std::string &Name, std::vector<std::string> Args);

        const std::string &getName() const;
        Function *codegen();
    };

    /// FunctionAST - This class represents a function definition itself.
    class FunctionAST
    {
        PrototypeAST *Proto;
        ExprAST *Body;

    public:
        FunctionAST(PrototypeAST *Proto, ExprAST *Body);
        Function *codegen();
    };

    /// IfExprAST - Expression class for if/then/else.
    class IfExprAST : public ExprAST {
        ExprAST* Cond;
        ExprAST* Then;
        ExprAST* Else;

    public:
        IfExprAST(ExprAST* Cond, ExprAST* Then, ExprAST* Else);

        Value* codegen() override;
    };

    /// ForExprAST - Expression class for for/in.
    class ForExprAST : public ExprAST {
        std::string VarName;
        ExprAST* Start; 
        ExprAST* End; 
        ExprAST* Step; 
        ExprAST* Body;

    public:
        ForExprAST(const std::string& VarName, ExprAST* Start, ExprAST* End, ExprAST* Step, ExprAST* Body);

        Value* codegen() override;
    };

    void createExternalFunctions();
}