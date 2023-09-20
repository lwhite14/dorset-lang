#include "ast.h"

namespace AST
{
    NumberExprAST::NumberExprAST(double Val) : Val(Val)
    {
    }

    VariableExprAST::VariableExprAST(const std::string &Name) 
        : Name(Name)
    {
    }

    BinaryExprAST::BinaryExprAST(char Op, ExprAST *LHS, ExprAST *RHS) 
        : Op(Op), LHS(LHS), RHS(RHS)
    {
    }

    CallExprAST::CallExprAST(const std::string &Callee, std::vector<ExprAST *> Args) 
        : Callee(Callee), Args(Args)
    {
    }

    PrototypeAST::PrototypeAST(const std::string &Name, std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args))
    {
    }

    const std::string &PrototypeAST::getName() const
    {
        return Name;
    }

    FunctionAST::FunctionAST(PrototypeAST *Proto, ExprAST *Body)
        : Proto(Proto), Body(Body)
    {
    }
}