#pragma once

#include <string>
#include <map>

enum TokenType 
{
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUNCTION, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE, EXTERN,
    THEN, IN,

    // Special
    _EOF, TERMINATING
};

static std::map<std::string, TokenType> keywords = {
    {"and",    AND},
    {"class",  CLASS},
    {"else",   ELSE},
    {"false",  FALSE},
    {"for",    FOR},
    {"fn",     FUNCTION},
    {"if",     IF},
    {"nil",    NIL},
    {"or",     OR},
    {"print",  PRINT},
    {"return", RETURN},
    {"super",  SUPER},
    {"this",   THIS},
    {"true",   TRUE},
    {"var",    VAR},
    {"while",  WHILE},
    {"extern", EXTERN},
    {"then",   THEN},
    {"in",     IN}
};

class Token 
{
private:
    TokenType type;
    std::string lexeme;
    std::string literal;
    int line; 
    int character;

public:
    Token(TokenType type, std::string lexeme, std::string literal, int line, int character);

    TokenType getType();
    std::string getTypeStr();
    std::string getLexeme();
    std::string getLiteral();
    int getLine();
    int getCharacter();
};