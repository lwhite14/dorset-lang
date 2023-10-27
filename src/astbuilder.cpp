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
        case EXTERN:
            handleExtern();
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
    std::string FnName;

    unsigned Kind = 0; // 0 = identifier, 1 = unary, 2 = binary.
    unsigned BinaryPrecedence = 30;

    switch (currentToken().getType())
    {
    default:
        ErrorHandler::error("expected function name in prototype", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    case IDENTIFIER:
        FnName = currentToken().getLexeme();
        Kind = 0;
        advanceToken();
        break;
    case UNARY:
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
        break;
    case BINARY:
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
        break;
    }

    if (currentToken().getType() != LEFT_PAREN)
    {
        ErrorHandler::error("expected '(' in prototype", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

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

    // Verify right number of names for operator.
    if (Kind && ArgNames.size() != Kind)
    {
        ErrorHandler::error("invalid number of operands for operator", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    return new AST::PrototypeAST(FnName, std::move(ArgNames), Kind != 0, BinaryPrecedence);
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
    }
    else
    {
        // Skip token for error recovery.
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
    }
    else
    {
        // Skip token for error recovery.
        advanceToken();
    }
}
