#include "token.h"

#include "../deps/magic_enum.hpp"

Token::Token(TokenType type, std::string lexeme, std::string* literal, int line) 
{
    this->type = type;
    this->lexeme = lexeme;
    this->literal = literal;
    this->line = line;
}

TokenType Token::getType()
{
    return type;
}

std::string Token::getTypeStr()
{
    return std::string(magic_enum::enum_name(type));
}

std::string Token::getLexeme()
{
    return lexeme;
}

std::string Token::getLiteral()
{
    if (literal == nullptr)
    {
        return "";
    }
    return *literal;
}

int Token::getLine()
{
    return line;
}
