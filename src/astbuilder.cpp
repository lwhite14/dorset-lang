#include "astbuilder.h"

ASTBuilder::ASTBuilder(std::vector<Token> tokens)
{
    this->tokens = tokens;
    this->currentTokenIndex = 0;
}

Token ASTBuilder::currentToken()
{
    return tokens[currentTokenIndex];
}

Token ASTBuilder::advanceToken()
{
    currentTokenIndex++;
    return currentToken();
}

int ASTBuilder::getTokPrecedence()
{
    if (currentToken().getLexeme().length() != 1)
    {
        return -1;
    }

    if (!isascii(currentToken().getLexeme()[0]))
    {
        return -1;
    }

    // Make sure it's a declared binop.
    int TokPrec = binopPrecedence[currentToken().getLexeme()[0]];
    if (TokPrec <= 0)
    {
        return -1;
    }
    return TokPrec;
}

AST::ExprAST *ASTBuilder::logError(Token token, std::string message)
{
    ErrorHandler::error(message, token.getLine(), token.getCharacter());
    return nullptr;
}

AST::PrototypeAST *ASTBuilder::logErrorP(Token token, std::string message)
{
    ErrorHandler::error(message, token.getLine(), token.getCharacter());
    return nullptr;
}

void ASTBuilder::parseTokenList(std::string fileName, std::string filePath)
{
    AST::initializeModule(fileName, filePath);
    AST::createExternalFunctions();
    while (true)
    {
        switch (currentToken().getType())
        {
        case _EOF:
            if (!ErrorHandler::HadError) 
            {
                AST::outputModule();
            }
            return;
        case SEMICOLON: // ignore top-level semicolons.
            advanceToken();
            break;
        case FUNCTION:
            handleDefinition();
            break;
        case EXTERN:
            handleExtern();
            break;
        default:
            handleTopLevelExpression();
            break;
        }
    }
}

AST::ExprAST *ASTBuilder::parseNumberExpr()
{
    AST::NumberExprAST *output = new AST::NumberExprAST(std::stod(currentToken().getLiteral()));
    advanceToken();
    return output;
}

AST::ExprAST *ASTBuilder::parseStringExpr()
{
    AST::StringExprAST *output = new AST::StringExprAST(currentToken().getLiteral());
    advanceToken();
    return output;
}

AST::ExprAST *ASTBuilder::parseParenExpr()
{
    advanceToken(); // eat (.
    AST::ExprAST *V = parseExpression();
    if (!V)
    {
        return nullptr;
    }

    if (currentToken().getType() != RIGHT_PAREN)
    {
        return logError(currentToken(), "expected ')'");
    }
    advanceToken(); // eat ).
    return V;
}

AST::ExprAST *ASTBuilder::parseIdentifierExpr()
{
    std::string IdName = currentToken().getLexeme();

    advanceToken(); // eat identifier.

    if (currentToken().getType() != LEFT_PAREN) // Simple variable ref.
        return new AST::VariableExprAST(IdName);

    // Call.
    advanceToken(); // eat (
    std::vector<AST::ExprAST *> Args;
    if (currentToken().getType() != RIGHT_PAREN)
    {
        while (true)
        {
            if (AST::ExprAST *Arg = parseExpression())
            {
                Args.push_back(Arg);
            }
            else
            {
                return nullptr;
            }

            if (currentToken().getType() == RIGHT_PAREN)
            {
                break;
            }

            if (currentToken().getType() != COMMA)
            {
                return logError(currentToken(), "Expected ')' or ',' in argument list");
            }
            advanceToken();
        }
    }

    // Eat the ')'.
    advanceToken();

    return new AST::CallExprAST(IdName, Args);
}

AST::ExprAST *ASTBuilder::parsePrimary()
{
    switch (currentToken().getType())
    {
    default:
        return logError(currentToken(), "unknown token when expecting an expression, token type: " + currentToken().getTypeStr());
    case IDENTIFIER:
        return parseIdentifierExpr();
    case NUMBER:
        return parseNumberExpr();
    case STRING:
        return parseStringExpr();
    case LEFT_PAREN:
        return parseParenExpr();
    }
}

AST::ExprAST *ASTBuilder::parseExpression()
{
    auto LHS = parsePrimary();
    if (!LHS)
    {
        return nullptr;
    }

    return parseBinOpRHS(0, LHS);
}

AST::ExprAST *ASTBuilder::parseBinOpRHS(int ExprPrec, AST::ExprAST *LHS)
{
    // If this is a binop, find its precedence.
    while (true)
    {
        int TokPrec = getTokPrecedence();

        // If this is a binop that binds at least as tightly as the current binop,
        // consume it, otherwise we are done.
        if (TokPrec < ExprPrec)
        {
            return LHS;
        }

        // Okay, we know this is a binop.
        int BinOp = currentToken().getLexeme()[0];
        advanceToken(); // eat binop

        // Parse the primary expression after the binary operator.
        auto RHS = parsePrimary();
        if (!RHS)
        {
            return nullptr;
        }

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int NextPrec = getTokPrecedence();
        if (TokPrec < NextPrec)
        {
            RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS)
            {
                return nullptr;
            }
        }

        // Merge LHS/RHS.
        LHS = new AST::BinaryExprAST(BinOp, LHS, RHS);
    }
}

AST::PrototypeAST *ASTBuilder::parsePrototype()
{
    if (currentToken().getType() != IDENTIFIER)
    {
        return logErrorP(currentToken(), "Expected function name in prototype");
    }

    std::string FnName = currentToken().getLexeme();
    advanceToken();

    if (currentToken().getType() != LEFT_PAREN)
    {
        return logErrorP(currentToken(), "Expected '(' in prototype");
    }

    // Read the list of argument names.
    std::vector<std::string> ArgNames;
    while (advanceToken().getType() == IDENTIFIER)
    {
        ArgNames.push_back(currentToken().getLexeme());
    }
    if (currentToken().getType() != RIGHT_PAREN)
    {
        return logErrorP(currentToken(), "Expected ')' in prototype");
    }

    // success.
    advanceToken(); // eat ')'.

    return new AST::PrototypeAST(FnName, ArgNames);
}

AST::FunctionAST *ASTBuilder::parseDefinition()
{
    advanceToken(); // eat def.
    auto Proto = parsePrototype();
    if (!Proto)
    {
        return nullptr;
    }

    if (auto E = parseExpression())
    {
        return new AST::FunctionAST(Proto, E);
    }
    return nullptr;
}

AST::PrototypeAST *ASTBuilder::parseExtern()
{
    advanceToken(); // eat extern.
    return parsePrototype();
}

AST::FunctionAST *ASTBuilder::parseTopLevelExpr()
{
    if (auto E = parseExpression())
    {
        // Make an anonymous proto.
        auto Proto = new AST::PrototypeAST("", std::vector<std::string>());
        return new AST::FunctionAST(Proto, E);
    }
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

void ASTBuilder::handleTopLevelExpression()
{
    // Evaluate a top-level expression into an anonymous function.
    if (auto FnAST = parseTopLevelExpr())
    {
        if (auto *FnIR = FnAST->codegen())
        {

            // Remove the anonymous expression.
            FnIR->eraseFromParent();
        }
    }
    else
    {
        // Skip token for error recovery.
        advanceToken();
    }
}
