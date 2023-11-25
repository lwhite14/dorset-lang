#pragma once

#include <string>
#include <vector>

#include "../LexicalAnalysis/Token.h"
#include "../AST/AST.h"
#include "../Utils/Error.h"
#include "../Builder/ExpressionBuilder.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

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