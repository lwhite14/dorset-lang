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

    while (currentToken().getType() != SEMICOLON)
    {
        if (currentToken().getType() == _EOF)
        {
            ErrorHandler::error("an expression has unexpectedly reached the end of the file", currentToken().getLine());
            return nullptr;
        }

        if (currentToken().getType() == RETURN) 
        {
            hasReturnToken = true;
        }

        exprTokens.push_back(currentToken());
        advanceToken();
    }
    exprTokens.push_back(Token(_EOE, " ", "", currentToken().getLine(), currentToken().getCharacter()));
    advanceToken(); // Eat semicolon
    ExpressionBuilder builder = ExpressionBuilder(exprTokens, needsReturnToken);
    return builder.buildExpression();
}

AST::ExprAST *ASTBuilder::parseIfExpression()
{
    advanceToken();  // eat the if.

    // condition.
    if (currentToken().getType() != LEFT_PAREN)
    {
        ErrorHandler::error("expected open parentheses", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken(); // eat '('

    std::vector<Token> exprTokens;
    while (currentToken().getType() != RIGHT_PAREN)
    {
        exprTokens.push_back(currentToken());
        advanceToken();
    }
    exprTokens.push_back(Token(_EOE, " ", "", currentToken().getLine(), currentToken().getCharacter()));
    advanceToken(); // eat ')'
    ExpressionBuilder builder = ExpressionBuilder(exprTokens, false);
    AST::ExprAST* Cond = builder.buildExpression();

    if (!Cond)
    {
        return nullptr;
    }

    bool thenReturns = false;
    if (currentToken().getType() == RETURN)
    {
        thenReturns = true;
    } // TODO: this will always return false, need to refactor

    AST::BlockAST* Then = parseBlock();
    if (!Then)
    {
        return nullptr;
    }

    if (currentToken().getType() != ELSE)
    {
        ErrorHandler::error("expected else", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken(); // eat 'else'

    bool elseReturns = false;
    if (currentToken().getType() == RETURN)
    {
        elseReturns = true;
    } // TODO: this will always return false, need to refactor

    AST::BlockAST* Else = parseBlock();
    if (!Else)
    {
        return nullptr;
    }

    return new AST::IfExprAST(std::move(Cond), std::move(Then), std::move(Else), thenReturns, elseReturns);
}

AST::ExprAST *ASTBuilder::parseForExpression()
{
    advanceToken();  // eat the for.

    if (currentToken().getType() != LEFT_PAREN)
    {
        ErrorHandler::error("expected open parentheses", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken(); // eat '('

    if (currentToken().getType() != IDENTIFIER) 
    {
        ErrorHandler::error("expected identifier after for", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    std::string IdName = currentToken().getLexeme();
    advanceToken();  // eat identifier.

    if (currentToken().getLexeme() != "=") 
    {
        ErrorHandler::error("expected '=' after for", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken();  // eat '='.

     
    std::vector<Token> startExprTokens;
    while (currentToken().getType() != COMMA)
    {
        startExprTokens.push_back(currentToken());
        advanceToken();
    }
    startExprTokens.push_back(Token(_EOE, " ", "", currentToken().getLine(), currentToken().getCharacter()));
    advanceToken(); // eat ','
    ExpressionBuilder builder = ExpressionBuilder(startExprTokens, false);
    AST::ExprAST* Start = builder.buildExpression();

    if (!Start) 
    {
        return nullptr;
    }

    std::vector<Token> endExprTokens;
    while (currentToken().getType() != COMMA)
    {
        endExprTokens.push_back(currentToken());
        advanceToken();
    }
    endExprTokens.push_back(Token(_EOE, " ", "", currentToken().getLine(), currentToken().getCharacter()));
    advanceToken(); // eat ','
    builder = ExpressionBuilder(endExprTokens, false);
    AST::ExprAST* End = builder.buildExpression();

    if (!End)
    {
        return nullptr;
    }

    // The step
    std::vector<Token> stepExprTokens;
    while (currentToken().getType() != RIGHT_PAREN)
    {
        stepExprTokens.push_back(currentToken());
        advanceToken();
    }
    stepExprTokens.push_back(Token(_EOE, " ", "", currentToken().getLine(), currentToken().getCharacter()));
    advanceToken(); // eat ')'
    builder = ExpressionBuilder(stepExprTokens, false);
    AST::ExprAST* Step = builder.buildExpression();
    if (!Step)
    {
        return nullptr;
    }

    AST::BlockAST* Body = parseBlock();

    if (!Body)
    {
        return nullptr;
    }

    return new AST::ForExprAST(IdName, std::move(Start), std::move(End), std::move(Step), std::move(Body));
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

    AST::BlockAST* block = parseBlock();

    if (block == nullptr)
    {
        ErrorHandler::error("a block has not been parsed correctly", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    if (needsReturnToken && !hasReturnToken)
    {
        ErrorHandler::error("this function needs a return", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    AST::FunctionAST* Function = new AST::FunctionAST(std::move(Proto), block);
    hasReturnToken = false;
    needsReturnToken = true;
    
    return std::move(Function);
}

AST::BlockAST *ASTBuilder::parseBlock()
{   
    if (currentToken().getType() != LEFT_BRACE)
    {
        ErrorHandler::error("unknown token when expecting opening brace at the start of the function", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken(); // Eat '{'

    std::vector<AST::ExprAST*> Exprs;

    while (currentToken().getType() != RIGHT_BRACE)
    {
        if (currentToken().getType() == _EOF)
        {
            ErrorHandler::error("a block has unexpectedly reached the end of the file", currentToken().getLine());
            return nullptr;
        }

        if (currentToken().getType() == LEFT_BRACE)
        {
            AST::BlockAST* block = parseBlock();
            Exprs.push_back(block);
        }
        else if (currentToken().getType() == IF)
        {
            AST::ExprAST* expr = parseIfExpression();
            Exprs.push_back(expr);
        }
        else if (currentToken().getType() == FOR)
        {
            AST::ExprAST* expr = parseForExpression();
            Exprs.push_back(expr);
        }
        else
        {
            AST::ExprAST* expr = parseExpression();
            Exprs.push_back(expr);
        }
    }

    advanceToken(); // Eat '}'

    return new AST::BlockAST(Exprs);
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
