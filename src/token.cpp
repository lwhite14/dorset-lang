#include "token.h"

#include "../deps/magic_enum.hpp"

Token::Token(TokenType type, std::string lexeme, int line) 
{
    this->type = type;
    this->lexeme = lexeme;
    this->line = line;
}

std::string Token::toString() 
{
    return "|" + std::string(magic_enum::enum_name(type)) + "| --- |" + lexeme + "| --- |" + std::to_string(line) + "|";
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

int Token::getLine()
{
    return line;
}
