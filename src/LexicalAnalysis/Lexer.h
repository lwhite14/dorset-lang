#pragma once

#include <string>
#include <vector>

#include "Token.h"
#include "../Utils/Error.h"

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
    void addToken(enum TokenType type);
    void addToken(enum TokenType type, std::string literal);
    bool match(char expected) ;
    char peek();
    void string();
    void number();
    char peekNext();
    void nonKeywordAlpha();
    bool identifier(std::string text);
    bool type(std::string text);
    void nextLine();

public:
    Lexer(std::string source);
    std::vector<Token> scanTokens();

};