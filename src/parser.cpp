#include "parser.h"

Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;
    this->currentTokenIndex = 0;
}

Token Parser::currentToken()
{
    return tokens[currentTokenIndex];
}

Token Parser::advanceToken()
{
    currentTokenIndex++;
    return currentToken();
}

