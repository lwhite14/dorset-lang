#pragma once

#include <string>
#include <vector>

#include "token.h"

class Scanner
{
private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::string* literal);
    bool match(char expected) ;
    char peek();
    void string();
    void number();
    char peekNext();
    void identifier();

public:
    Scanner(std::string source);
    std::vector<Token> scanTokens();

};