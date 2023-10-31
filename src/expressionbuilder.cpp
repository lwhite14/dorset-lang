#include "expressionbuilder.h"

#include "error.h"

ExpressionBuilder::ExpressionBuilder(std::vector<Token> tokens)
{
    this->tokens = tokens;
    this->currentTokenIndex = 0;
}

Token ExpressionBuilder::currentToken()
{
    if (currentTokenIndex >= tokens.size())
    {
        ErrorHandler::error("overshot token list length in expression, this can be caused by a miriad of issues", tokens[tokens.size() - 1].getLine());
        exit(1);
    }
    return tokens[currentTokenIndex];
}

Token ExpressionBuilder::advanceToken()
{
    currentTokenIndex++;
    return currentToken();
}

AST::ExprAST *ExpressionBuilder::buildExpression()
{
    auto LHS = parseUnary();
    if (!LHS)
    {
        return nullptr;
    }

    return parseBinOpRHS(0, std::move(LHS));
}


AST::ExprAST *ExpressionBuilder::parseNumberExpr()
{
    AST::NumberExprAST *output = new AST::NumberExprAST(std::stod(currentToken().getLiteral()));
    advanceToken();
    return std::move(output);
}

AST::ExprAST *ExpressionBuilder::parseStringExpr()
{
    AST::StringExprAST *output = new AST::StringExprAST(currentToken().getLiteral());
    advanceToken();
    return std::move(output);
}

AST::ExprAST *ExpressionBuilder::parseParenExpr()
{
    advanceToken(); // eat (.

    AST::ExprAST *V = buildExpression();
    if (!V)
    {
        return nullptr;
    }

    if (currentToken().getType() != RIGHT_PAREN)
    {
        ErrorHandler::error("expected ')'", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
    advanceToken(); // eat ).
    return V;
}

AST::ExprAST *ExpressionBuilder::parseIdentifierExpr()
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
            if (AST::ExprAST *Arg = buildExpression())
            {
                Args.push_back(std::move(Arg));
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
                ErrorHandler::error("expected ')' or ',' in argument list", currentToken().getLine(), currentToken().getCharacter());
                return nullptr;
            }
            advanceToken();
        }
    }

    // Eat the ')'.
    advanceToken();

    return new AST::CallExprAST(IdName, std::move(Args));
}

AST::ExprAST* ExpressionBuilder::parseIfExpr()
{
    advanceToken();  // eat the if.

    // condition.
    auto Cond = buildExpression();
    if (!Cond)
        return nullptr;

    if (currentToken().getType() != THEN)
    {
        ErrorHandler::error("expected then");
        return nullptr;
    }
    advanceToken();  // eat the then

    auto Then = buildExpression();
    if (!Then)
        return nullptr;

    if (currentToken().getType() != ELSE)
    {
        ErrorHandler::error("expected else");
        return nullptr;
    }

    advanceToken();

    auto Else = buildExpression();
    if (!Else)
        return nullptr;

    return new AST::IfExprAST(std::move(Cond), std::move(Then),
        std::move(Else));
}

AST::ExprAST* ExpressionBuilder::parseForExpr()
{
    advanceToken();  // eat the for.

    if (currentToken().getType() != IDENTIFIER) 
    {
        ErrorHandler::error("expected identifier after for");
        return nullptr;
    }

    std::string IdName = currentToken().getLexeme();
    advanceToken();  // eat identifier.

    if (currentToken().getLexeme() != "=") 
    {
        ErrorHandler::error("expected '=' after for");
        return nullptr;
    }
    advanceToken();  // eat '='.

     
    auto Start = buildExpression();
    if (!Start) {
        return nullptr;
    }

    if (currentToken().getLexeme() != ",") {
        ErrorHandler::error("expected ',' after for start value");
        return nullptr;
    }
    advanceToken();

    auto End = buildExpression();
    if (!End)
        return nullptr;

    // The step value is optional.
    AST::ExprAST* Step = nullptr;
    if (currentToken().getLexeme() == ",") {
        advanceToken();
        Step = buildExpression();
        if (!Step)
            return nullptr;
    }

    if (currentToken().getType() != IN) {
        ErrorHandler::error("expected 'in' after for");
        return nullptr;
    }
    advanceToken();  // eat 'in'.

    auto Body = buildExpression();
    if (!Body)
        return nullptr;

    return new AST::ForExprAST(IdName, std::move(Start), std::move(End), std::move(Step), std::move(Body));
}

AST::ExprAST *ExpressionBuilder::parseVarExpr() 
{
    advanceToken(); // eat the var.

    // At least one variable name is required.
    if (currentToken().getType() != IDENTIFIER) {
        ErrorHandler::error("expected identifier after var", currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }

    std::string Name = currentToken().getLexeme();
    advanceToken(); // eat identifier.

    // Read the optional initializer.
    AST::ExprAST* Init = nullptr;
    if (currentToken().getLexeme()[0] == '=') {
        advanceToken(); // eat the '='.

        Init = parseUnary();
        if (!Init)
        {
            ErrorHandler::error("failed to build initializer value", currentToken().getLine(), currentToken().getCharacter());
            return nullptr;
        }
    }

    return new AST::VarExprAST(Name, std::move(Init));
}

AST::ExprAST *ExpressionBuilder::parsePrimary()
{
    if (currentToken().getType() == IDENTIFIER)
    {
        auto Expr = parseIdentifierExpr();
        return Expr;
    }
    else if (currentToken().getType() == NUMBER)
    {
        auto Expr = parseNumberExpr();
        return Expr;
    }
    else if (currentToken().getType() == STRING)
    {
        auto Expr = parseStringExpr();
        return Expr;
    }
    else if (currentToken().getType() == LEFT_PAREN)
    {
        auto Expr = parseParenExpr();
        return Expr;
    }
    else if (currentToken().getType() == IF)
    {
        auto Expr = parseIfExpr();
        return Expr;
    }
    else if (currentToken().getType() == FOR)
    {
        auto Expr = parseForExpr();
        return Expr;
    }
    else if (currentToken().getType() == VAR) 
    {
        auto Expr = parseVarExpr();
        return Expr;
    }
    else
    {
        ErrorHandler::error("unknown token when expecting an expression: " + currentToken().getLexeme(), currentToken().getLine(), currentToken().getCharacter());
        return nullptr;
    }
}

AST::ExprAST *ExpressionBuilder::parseBinOpRHS(int ExprPrec, AST::ExprAST *LHS)
{
    // If this is a binop, find its precedence.
    while (true) 
    {
        int TokPrec = getTokPrecedence();

        // If this is a binop that binds at least as tightly as the current binop,
        // consume it, otherwise we are done.
        if (TokPrec < ExprPrec)
            return LHS;

        // Okay, we know this is a binop.
        int BinOp = currentToken().getLexeme()[0];
        advanceToken(); // eat binop

        // Parse the unary expression after the binary operator.
        auto RHS = parseUnary();
        if (!RHS)
            return nullptr;

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int NextPrec = getTokPrecedence();
        if (TokPrec < NextPrec) {
            RHS = parseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }
        
        // Merge LHS/RHS.
        LHS = new AST::BinaryExprAST(BinOp, std::move(LHS), std::move(RHS));
    }
}

AST::ExprAST* ExpressionBuilder::parseUnary()
{
    // If the current token is not an operator, it must be a primary expr.
    if (!isOperator(currentToken().getLexeme()[0]) || currentToken().getType() == LEFT_PAREN || currentToken().getType() == COMMA)
    {
        return parsePrimary();
    }

    // If this is a unary operator, read it.
    int Opc = currentToken().getLexeme()[0];
    advanceToken();
    if (auto Operand = parseUnary())
        return new AST::UnaryExprAST(Opc, std::move(Operand));
    return nullptr;
}

int ExpressionBuilder::getTokPrecedence()
{
    if (currentToken().getType() == _EOE)
    {
        return -1;
    }

    if (currentToken().getLexeme().length() != 1)
    {
        return -1;
    }

    if (!isascii(currentToken().getLexeme()[0]))
    {
        return -1;
    }

    // Make sure it's a declared binop.
    int TokPrec = AST::MasterAST::BinopPrecedence[currentToken().getLexeme()[0]];
    if (TokPrec <= 0)
    {
        return -1;
    }
    return TokPrec;
}