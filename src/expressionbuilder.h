#pragma once

#include <vector>

#include "token.h"
#include "ast.h"

class ExpressionBuilder
{
private:
    std::map<char, int> binopPrecedence = {
        {'<', 10},
        {'+', 20},
        {'-', 30},
        {'*', 40}};

    std::vector<Token> tokens;
    int currentTokenIndex;

    Token currentToken();
    Token advanceToken();

    AST::ExprAST *parseNumberExpr();
    AST::ExprAST *parseStringExpr();
    AST::ExprAST *parseParenExpr();
    AST::ExprAST *parseIdentifierExpr();
    AST::ExprAST *parsePrimary();
    AST::ExprAST *parseBinOpRHS(int ExprPrec, AST::ExprAST *LHS);
    int getTokPrecedence();

public:
    ExpressionBuilder(std::vector<Token> tokens);

    AST::ExprAST *buildExpression();
};