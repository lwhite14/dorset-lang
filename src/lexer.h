#pragma once

#include <string>
#include <vector>

#include "token.h"

class Lexer
{
private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int charactersAtLineStart = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::string literal);
    bool match(char expected) ;
    char peek();
    void string();
    void number();
    char peekNext();
    void identifier();
    void nextLine();

public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();

};