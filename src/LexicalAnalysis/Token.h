#pragma once

#include <string>
#include <map>

enum TokenType 
{
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, SEMICOLON, LEFT_SQUARE, RIGHT_SQUARE,

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

    // Operator Related
    BINARY, UNARY,

    // Keywords.
    AND, CLASS, ELSE, _FALSE, FUNCTION, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, _TRUE, VAR, WHILE, EXTERN,
    THEN, _IN, 

    // Types
    TYPE_VOID, TYPE_DOUBLE,

    // Special
    _EOE, // End of expression
    _EOF // End of file
};

static std::map<std::string, enum TokenType> keywords = {
    {"and",    AND},
    {"class",  CLASS},
    {"else",   ELSE},
    {"false",  _FALSE},
    {"for",    FOR},
    {"fn",     FUNCTION},
    {"if",     IF},
    {"nil",    NIL},
    {"or",     OR},
    {"print",  PRINT},
    {"return", RETURN},
    {"super",  SUPER},
    {"this",   THIS},
    {"true",   _TRUE},
    {"var",    VAR},
    {"while",  WHILE},
    {"extern", EXTERN},
    {"then",   THEN},
    {"in",     _IN},
    {"binary", BINARY},
    {"unary",  UNARY},
};

static std::map<std::string, enum TokenType> types = {
    {"void",    TYPE_VOID},
    {"double",  TYPE_DOUBLE}
};

class Token 
{
private:
    enum TokenType type;
    std::string lexeme;
    std::string literal;
    int line; 
    int character;

public:
    Token(enum TokenType type, std::string lexeme, std::string literal, int line, int character);

    enum TokenType getType();
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