#pragma once

#include <string>
#include <map>

enum TokenType 
{
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, SEMICOLON,

    // One or two character tokens.
                EXCLAMATION_EQUAL,
    EQUAL,      EQUAL_EQUAL,
                GREATER_EQUAL,
                LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Operators
    PLUS, SLASH, STAR, FORWARD_SLASH, BACK_SLASH, VERTICAL_BAR, 
    COLON, AMPERSAND, CARET, EXCLAMATION, LESS, GREATER, 

    // Keywords.
    AND, CLASS, ELSE, FALSE, FUNCTION, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE, EXTERN,
    THEN, IN,

    // Special
    _EOF, TERMINATING,

    // Operator Related
    BINARY, UNARY
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
    {"in",     IN},
    {"binary", BINARY},
    {"unary",  UNARY}
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

static bool isOperator(char c) 
{
    if (c == '+') { return true; }
    if (c == '-') { return true; }
    if (c == '*') { return true; }
    if (c == '/') { return true; }
    if (c == '\\') { return true; }
    if (c == '|') { return true; }
    if (c == ':') { return true; }
    if (c == '&') { return true; }
    if (c == '^') { return true; }
    if (c == '!') { return true; }
    if (c == '<') { return true; }
    if (c == '>') { return true; }
    return false;
}