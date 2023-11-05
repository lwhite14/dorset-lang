#include "astbuilder.h"

ASTBuilder::ASTBuilder(std::vector<Token> tokens)
{
    this->tokens = tokens;
    this->currentTokenIndex = 0;
}

Token ASTBuilder::currentToken()
{
    if (currentTokenIndex >= tokens.size())
    {
        ErrorHandler::error("overshot token list length in source root, this can be caused by a miriad of issues");
        exit(1);
    }
    return tokens[currentTokenIndex];
}

Token ASTBuilder::advanceToken()
{
    currentTokenIndex++;
    return currentToken();
}

void ASTBuilder::parseTokenList()
{
    while (true)
    {
        if (currentToken().getType() == _EOF)
        {
            return;
        }
        else if (currentToken().getType() == FUNCTION)
        {
            handleDefinition();
        }
        else if (currentToken().getType() == EXTERN)
        {
            handleExtern();
        }
        else
        {
            ErrorHandler::error("unexpected token at 'top level'", currentToken().getLine(), currentToken().getCharacter());
            bool cond = true;
            while (cond)
            {
                advanceToken();
                if (currentToken().getType() == _EOF || currentToken().getType() == FUNCTION || currentToken().getType() == EXTERN)
                {
                    cond = false;
                }
            }
        }
    }
}

AST::ExprAST *ASTBuilder::parseExpression()
{
    std::vector<Token> exprTokens;
    bool cond = true;
    int blockCounter = 1;

    while (cond)
    {
        if (currentToken().getType() == RIGHT_BRACE) 
        {
            blockCounter--;
        }
        if (currentToken().getType() == LEFT_BRACE)
        {
            blockCounter++;
        }

        if (currentToken().getType() == _EOF || blockCounter == 0)
        {
            ErrorHandler::error("a block hasn't been terminated");
            return nullptr;
        }

        if (currentToken().getType() == RETURN) 
        {
            hasReturnToken = true;
        }

        if (currentToken().getType() == SEMICOLON && blockCounter == 1) 
        {
            cond = false;
        }
        else 
        {
            exprTokens.push_back(currentToken());
            advanceToken();
        }
    }
    exprTokens.push_back(Token(_EOE, " ", "", currentToken().getLine(), currentToken().getCharacter()));
    advanceToken();
    ExpressionBuilder builder = ExpressionBuilder(exprTokens, needsReturnToken);
    return builder.buildExpression();
}

AST::PrototypeAST *ASTBuilder::parsePrototype()
{
    std::string FnName;

    unsigned Kind = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned BinaryPrecedence = 30;

    if (currentToken().getType() == IDENTIFIER)
    {
        FnName = currentToken().getLexeme();
        Kind = 0;
        advanceToken();
    }
    else if (currentToken().getType() == UNARY)
    {
        advanceToken();
        if (!isascii(currentToken().getLexeme()[0]))
        {
            ErrorHandler::error("expected unary operator", currentToken().getLine(), currentToken().getCharacter());
            return nullptr;
        }
        FnName = "unary";
        FnName += currentToken().getLexeme()[0];
        Kind = 1;
        advanceToken();
    }
    else if (currentToken().getType() == BINARY)
    {
        advanceToken();
        if (!isascii(currentToken().getLexeme()[0]))
        {
            ErrorHandler::error("expected binary operator", currentToken().getLine(), currentToken().getCharacter());
            return nullptr;
        }
        FnName = "binary";
        FnName += currentToken().getLexeme()[0];
        Kind = 2;
        advanceToken();

        // Read the precedence if present.
        if (currentToken().getType() == NUMBER)
        {
            if (std::stod(currentToken().getLiteral()) < 1 || std::stod(currentToken().getLiteral()) > 100)
            {
                ErrorHandler::error("invalid precedence: must be 1..100", currentToken().getLine(), currentToken().getCharacter());
                return nullptr;
            }
            BinaryPrecedence = (unsigned)std::stod(currentToken().getLiteral());
            advanceToken();
        }
    }
    else
    {
        ErrorHandler::error("expected function name in prototype", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    if (currentToken().getType() != LEFT_PAREN)
    {
        ErrorHandler::error("expected '(' in prototype", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    advanceToken(); // Move to first argument in argument list.
    std::vector<std::string> ArgNames;
    while (currentToken().getType() == IDENTIFIER)
    {
        ArgNames.push_back(currentToken().getLexeme());
        advanceToken(); // Move to comma or right parethesis
        if (currentToken().getType() != COMMA)
        {
            break;
        }
        advanceToken();
    }
    if (currentToken().getType() != RIGHT_PAREN)
    {
        ErrorHandler::error("expected ')' or ',' in prototype", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    // success.
    advanceToken(); // eat ')'.

    std::string returnType;
    if (currentToken().getType() == TYPE_VOID || currentToken().getType() == TYPE_DOUBLE) 
    {
        returnType = currentToken().getLexeme();
    }
    else 
    {
        ErrorHandler::error("expect return type here", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    advanceToken(); // eat type declaration.

    // Verify right number of names for operator.
    if (Kind && ArgNames.size() != Kind)
    {
        ErrorHandler::error("invalid number of operands for operator", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    return new AST::PrototypeAST(FnName, std::move(ArgNames), returnType, Kind != 0, BinaryPrecedence);
}

AST::FunctionAST *ASTBuilder::parseDefinition()
{
    advanceToken(); // eat def.
    auto Proto = parsePrototype();
    if (!Proto)
    {
        return nullptr;
    }

    needsReturnToken = true;
    if (Proto->getReturnType() == "void")
        needsReturnToken = false;

    // Eat '{'
    if (currentToken().getType() != LEFT_BRACE)
    {
        ErrorHandler::error("unknown token when expecting opening brace at the start of the function", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken();

    std::vector<AST::ExprAST*> expressions;

    hasReturnToken = false;
    while (currentToken().getType() != RIGHT_BRACE) 
    {
        auto* Expr = parseExpression();
        if (Expr == nullptr)
            return nullptr;

        expressions.push_back(Expr);
    }

    if (needsReturnToken && !hasReturnToken)
    {
        ErrorHandler::error("this function needs a return", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    // Eat '}'
    if (currentToken().getType() != RIGHT_BRACE)
    {
        ErrorHandler::error("unknown token when expecting closing brace at the end of the function", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    if (expressions.size() > 0)
    {
        // only advance past the right curly brace once we know the function hasn't
        // failed, this way the error recovery in handleDefinition works correctly
        advanceToken();
        
        return new AST::FunctionAST(std::move(Proto), expressions);
    }

    ErrorHandler::error("function cannot be empty, it needs a default return value", currentToken().getLine(), currentToken().getCharacter());
    return nullptr;
}

AST::PrototypeAST *ASTBuilder::parseExtern()
{
    advanceToken(); // eat extern.
    auto *Proto = parsePrototype();

    if (currentToken().getType() != SEMICOLON)
    {
        ErrorHandler::error("no terminating semicolon", currentToken().getLine());
        return nullptr;
    }
    advanceToken();

    return Proto;
}

void ASTBuilder::handleDefinition()
{
    if (auto FnAST = parseDefinition())
    {
        if (auto *FnIR = FnAST->codegen())
        {
        }
        else
        {
            ErrorHandler::error("codegen failed", currentToken().getLine());
        }
    }
    else
    {
        // Skip token for error recovery.
        if (currentTokenIndex < tokens.size() - 1)
            advanceToken();
    }
}

void ASTBuilder::handleExtern()
{
    if (auto ProtoAST = parseExtern())
    {
        if (auto *FnIR = ProtoAST->codegen())
        {
        }
        else
        {
            ErrorHandler::error("codegen failed", currentToken().getLine());
        }
    }
    else
    {
        // Skip token for error recovery.
        if (currentTokenIndex < tokens.size() - 1)
            advanceToken();
    }
}
