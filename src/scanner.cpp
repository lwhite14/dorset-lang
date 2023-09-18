#include "scanner.h"

#include "error.h"

Scanner::Scanner(std::string source) 
{
    this->source = source;
}

std::vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd()) 
    {
      start = current;
      scanToken();
    }

    tokens.push_back(Token(_EOF, "", line));
    return tokens;
}

void Scanner::scanToken()
{
    char c = advance();
    if (c == '(') { addToken(LEFT_PAREN); }
    else if (c == ')') { addToken(RIGHT_PAREN); }
    else if (c == '{') { addToken(LEFT_BRACE); }
    else if (c == '}') { addToken(RIGHT_BRACE); }
    else if (c == ',') { addToken(COMMA); }
    else if (c == '.') { addToken(DOT); }
    else if (c == '-') { addToken(SLASH); }
    else if (c == '+') { addToken(PLUS); }
    else if (c == ';') { addToken(SEMICOLON); }
    else if (c == '*') { addToken(STAR); }
    else if (c == '!') { addToken(match('=') ? BANG_EQUAL : BANG); }
    else if (c == '=') { addToken(match('=') ? EQUAL_EQUAL : EQUAL); }
    else if (c == '<') { addToken(match('=') ? LESS_EQUAL : LESS); }
    else if (c == '>') { addToken(match('=') ? GREATER_EQUAL : GREATER); }
    else if (c == '/')
    {
        if (match('/')) 
        {
          // A comment goes until the end of the line.
            while (peek() != '\n' && !isAtEnd()) advance();
        } 
        else 
        {
            addToken(SLASH);
        }
    }
    else if (c == ' ') {}
    else if (c == '\r') {}
    else if (c == '\t') {}
    else if (c == '\n') { line++; }
    else if (c == '"') { string(); }
    else if (c == '0') { if (match('r')) { addToken(OR); } }
    else 
    { 
        if (isDigit(c)) 
        {
            number();
        } 
        else if (isAlpha(c)) 
        {
            identifier();
        }
        else 
        {
            ErrorHandler::error(line, "Unexpected character.");        
        }
    }
}

bool Scanner::isAtEnd()
{
    return current >= source.size();
}

char Scanner::advance() 
{
    return source[current++];
}

void Scanner::addToken(TokenType type) 
{
    std::string text = source.substr(start, current);
    tokens.push_back(Token(type, text, line));
}

bool Scanner::match(char expected) 
{
    if (isAtEnd())
    { 
        return false;
    }

    if (source[current] != expected)
    {
        return false;
    }

    current++;
    return true;
}

char Scanner::peek() 
{
    if (isAtEnd()) 
    {
        return '\0';
    }

    return source[current];
}

void Scanner::string() 
{
    while (peek() != '"' && !isAtEnd()) 
    {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) 
    {
        ErrorHandler::error(line, "Unterminated string.");
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - 1);
    addToken(STRING);
}

bool Scanner::isDigit(char c) 
{
    return c >= '0' && c <= '9';
} 

void Scanner::number() 
{
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) advance();
    }

    addToken(NUMBER);
}

char Scanner::peekNext() 
{
    if (current + 1 >= source.length())
    {
        return '\0';
    }
    return source[current + 1];
} 

void Scanner::identifier() 
{
    while (isAlphaNumeric(peek())) advance();

    std::string text = source.substr(start, current);
    TokenType type;
    std::map<std::string, TokenType>::const_iterator pos = keywords.find(text);
    if (pos == keywords.end()) 
    {
        type = IDENTIFIER;
    } 
    else 
    {
        type = pos->second;
    }
    addToken(type);
}

bool Scanner::isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

bool Scanner::isAlphaNumeric(char c) 
{
    return isAlpha(c) || isDigit(c);
}