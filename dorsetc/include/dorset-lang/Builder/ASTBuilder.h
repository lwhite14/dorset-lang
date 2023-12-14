#pragma once

#include <string>
#include <vector>

#include <dorset-lang/LexicalAnalysis/Token.h>
#include <dorset-lang/AST/AST.h>
#include <dorset-lang/Utils/Error.h>
#include <dorset-lang/Builder/ExpressionBuilder.h>

namespace Dorset
{
    class ASTBuilder
    {
    private:
        std::vector<Token> tokens;
        int currentTokenIndex;
        bool needsReturnToken = false;

        Token currentToken();
        Token advanceToken();

        AST::ExprAST *parseExpression(bool& hasReturn);
        AST::PrototypeAST *parsePrototype(); 
        AST::FunctionAST *parseDefinition(); 
        AST::BlockAST *parseBlock(bool& hasReturn);
        AST::PrototypeAST *parseExtern();

        AST::ExprAST *parseIfExpression(bool& hasReturn);
        AST::ExprAST *parseForExpression(bool& hasReturn);


        void handleDefinition(); 
        void handleExtern();

    public:
        ASTBuilder(std::vector<Token> tokens);

        void parseTokenList();
    };
}