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
        ErrorHandler::error("overshot token list length \n\t\t this can be caused by a miriad of errors");
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
        switch (currentToken().getType())
        {
        case _EOF:
            return;
        case FUNCTION:
            handleDefinition();
            break;
        default:
            ErrorHandler::error("only functions allowed at the top level", currentToken().getLine(), currentToken().getCharacter());
            advanceToken(); // ignore non function tokens
            break;
        }
    }
}


AST::ExprAST *ASTBuilder::parseExpression()
{
    std::vector<Token> exprTokens;
    while (currentToken().getType() != SEMICOLON)
    {
        if (currentToken().getType() == _EOF || currentToken().getType() == RIGHT_BRACE)
        {
            ErrorHandler::error("no terminating semicolon", currentToken().getLine());
            return nullptr;
        }

        exprTokens.push_back(currentToken());
        advanceToken();
    }
    advanceToken();
    ExpressionBuilder builder = ExpressionBuilder(exprTokens);
    return builder.buildExpression();
}

AST::PrototypeAST *ASTBuilder::parsePrototype()
{
    if (currentToken().getType() != IDENTIFIER)
    {
        ErrorHandler::error("expected function name in prototype", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    std::string FnName = currentToken().getLexeme();
    advanceToken();

    if (currentToken().getType() != LEFT_PAREN)
    {
        ErrorHandler::error("expected '(' in prototype", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    // Read the list of argument names.
    std::vector<std::string> ArgNames;
    while (advanceToken().getType() == IDENTIFIER)
    {
        ArgNames.push_back(currentToken().getLexeme());
    }
    if (currentToken().getType() != RIGHT_PAREN)
    {
        ErrorHandler::error("expected ')' in prototype", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    // success.
    advanceToken(); // eat ')'.

    return new AST::PrototypeAST(FnName, std::move(ArgNames));
}

AST::FunctionAST *ASTBuilder::parseDefinition()
{
    advanceToken(); // eat def.
    auto Proto = parsePrototype();
    if (!Proto)
    {
        return nullptr;
    }

    // Eat '{'
    if (currentToken().getType() != LEFT_BRACE)
    {
        ErrorHandler::error("unknown token when expecting opening brace at the start of the function", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken();

    auto E = parseExpression();

    // Eat '}'
    if (currentToken().getType() != RIGHT_BRACE)
    {
        ErrorHandler::error("unknown token when expecting closing brace at the end of the function", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken();

    if (E)
        return new AST::FunctionAST(std::move(Proto), std::move(E));

    return nullptr;
}

void ASTBuilder::handleDefinition()
{
    if (auto FnAST = parseDefinition())
    {
        if (auto *FnIR = FnAST->codegen())
        {

        }
    }
    else
    {
        // Skip token for error recovery.
        advanceToken();
    }
}