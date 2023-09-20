#pragma once

#include <string>
#include <memory>
#include <vector>

namespace AST
{
    /// ExprAST - Base class for all expression nodes.
    class ExprAST
    {
    public:
        virtual ~ExprAST() = default;
    };

    /// NumberExprAST - Expression class for numeric literals like "1.0".
    class NumberExprAST : public ExprAST
    {
    private:
        double Val;

    public:
        NumberExprAST(double Val);
    };

    /// VariableExprAST - Expression class for referencing a variable, like "a".
    class VariableExprAST : public ExprAST
    {
    private:
        std::string Name;

    public:
        VariableExprAST(const std::string &Name);
    };

    /// BinaryExprAST - Expression class for a binary operator.
    class BinaryExprAST : public ExprAST
    {
    private:
        char Op;
        ExprAST* LHS;
        ExprAST* RHS;

    public:
        BinaryExprAST(char Op, ExprAST* LHS, ExprAST* RHS);
    };

    /// CallExprAST - Expression class for function calls.
    class CallExprAST : public ExprAST
    {
    private:
        std::string Callee;
        std::vector<ExprAST*> Args;

    public:
        CallExprAST(const std::string &Callee, std::vector<ExprAST*> Args);
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
    };

    /// FunctionAST - This class represents a function definition itself.
    class FunctionAST
    {
        PrototypeAST* Proto;
        ExprAST* Body;

    public:
        FunctionAST(PrototypeAST* Proto, ExprAST* Body);
    };
}