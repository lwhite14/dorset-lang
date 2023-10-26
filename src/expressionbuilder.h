#pragma once

#include <vector>

#include "token.h"
#include "ast.h"

class ExpressionBuilder
{
private:
    std::vector<Token> tokens;
    int currentTokenIndex;

    Token currentToken();
    Token advanceToken();

    AST::ExprAST *parseNumberExpr();
    AST::ExprAST *parseStringExpr();
    AST::ExprAST *parseParenExpr();
    AST::ExprAST *parseIdentifierExpr();
    AST::ExprAST *parseIfExpr();
    AST::ExprAST *parseForExpr();
    AST::ExprAST* parseVarExpr();
    AST::ExprAST *parsePrimary();
    AST::ExprAST *parseBinOpRHS(int ExprPrec, AST::ExprAST *LHS);
    AST::ExprAST* parseUnary();
    int getTokPrecedence();

public:
    ExpressionBuilder(std::vector<Token> tokens);

    AST::ExprAST *buildExpression();
};