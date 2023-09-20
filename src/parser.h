#pragma once

#include <string>
#include <vector>

#include "token.h"
#include "ast.h"
#include "error.h"

class Parser
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
    int getTokPrecedence();

    AST::ExprAST *logError(Token token, std::string message);
    AST::PrototypeAST *logErrorP(Token token, std::string message);

    AST::ExprAST *parseNumberExpr();
    AST::ExprAST *parseParenExpr();
    AST::ExprAST *parseIdentifierExpr();
    AST::ExprAST *parsePrimary();
    AST::ExprAST *parseExpression();
    AST::ExprAST *parseBinOpRHS(int ExprPrec, AST::ExprAST *LHS);
    AST::PrototypeAST *parsePrototype();
    AST::FunctionAST *parseDefinition();
    AST::PrototypeAST *parseExtern();
    AST::FunctionAST *parseTopLevelExpr();

    void handleDefinition();
    void handleExtern();
    void handleTopLevelExpression();

public:
    Parser(std::vector<Token> tokens);

    void parseTokenList();
};