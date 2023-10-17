#include "astbuilder.h"

#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"

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
    AST::MasterAST::initializeJIT();
    AST::MasterAST::initializeModule();
    AST::createExternalFunctions();
    while (true)
    {
        switch (currentToken().getType())
        {
        case _EOF:
            if (!ErrorHandler::HadError)
            {
                AST::MasterAST::outputModule();
                AST::MasterAST::removeBuildFiles();
            }
            return;
        case FUNCTION:
            handleDefinition();
            break;
        case EXTERN:
            handleExtern();
            break;
        default:
            ErrorHandler::error("unkown token at the top level", currentToken().getLine(), currentToken().getCharacter());
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
        return new AST::FunctionAST(Proto, E);

    return nullptr;
}

AST::PrototypeAST *ASTBuilder::parseExtern()
{
    advanceToken(); // eat extern.
    return parsePrototype();
}

void ASTBuilder::handleDefinition()
{
    if (auto FnAST = parseDefinition())
    {
        if (auto *FnIR = FnAST->codegen())
        {
            AST::MasterAST::ExitOnErr(AST::MasterAST::TheJIT->addModule(
                orc::ThreadSafeModule
                (
                    std::unique_ptr<Module>(AST::MasterAST::TheModule), 
                    std::unique_ptr<LLVMContext>(AST::MasterAST::TheContext)
                )));
            //AST::MasterAST::initializeModule();
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
            AST::MasterAST::FunctionProtos[ProtoAST->getName()] = std::move(ProtoAST);
        }
    }
    else
    {
        // Skip token for error recovery.
        advanceToken();
    }
}