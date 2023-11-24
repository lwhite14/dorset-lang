#pragma once

#include <vector>

#include "token.h"
#include "ast.h"

class ExpressionBuilder
{
private:
    std::vector<Token> tokens;
    int currentTokenIndex;
    bool needsReturnToken;

    Token currentToken();
    Token advanceToken();

    AST::ExprAST *parseNumberExpr();
    AST::ExprAST *parseStringExpr();
    AST::ExprAST *parseParenExpr();
    AST::ExprAST *parseIdentifierExpr();
    AST::ExprAST* parseVarExpr();
    AST::ExprAST* parseReturnExpr();
    AST::ExprAST *parsePrimary();
    AST::ExprAST *parseBinOpRHS(int ExprPrec, AST::ExprAST *LHS);
    AST::ExprAST* parseUnary();
    int getTokPrecedence();

public:
    ExpressionBuilder(std::vector<Token> tokens, bool needsReturnToken);

    AST::ExprAST *buildExpression();
};