#include "scanner.h"

#include <ctype.h>

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

    tokens.push_back(Token(_EOF, "", nullptr, line, start - charactersAtLineStart));
    return tokens;
}

void Scanner::scanToken()
{
    char c = advance();
    if (c == '(')
    {
        addToken(LEFT_PAREN);
    }
    else if (c == ')')
    {
        addToken(RIGHT_PAREN);
    }
    else if (c == '{')
    {
        addToken(LEFT_BRACE);
    }
    else if (c == '}')
    {
        addToken(RIGHT_BRACE);
    }
    else if (c == ',')
    {
        addToken(COMMA);
    }
    else if (c == '.')
    {
        addToken(DOT);
    }
    else if (c == '-')
    {
        addToken(SLASH);
    }
    else if (c == '+')
    {
        addToken(PLUS);
    }
    else if (c == ';')
    {
        addToken(SEMICOLON);
    }
    else if (c == '*')
    {
        addToken(STAR);
    }
    else if (c == '!')
    {
        addToken(match('=') ? BANG_EQUAL : BANG);
    }
    else if (c == '=')
    {
        addToken(match('=') ? EQUAL_EQUAL : EQUAL);
    }
    else if (c == '<')
    {
        addToken(match('=') ? LESS_EQUAL : LESS);
    }
    else if (c == '>')
    {
        addToken(match('=') ? GREATER_EQUAL : GREATER);
    }
    else if (c == '/')
    {
        if (match('/'))
        {
            while (peek() != '\n' && !isAtEnd())
                advance();
        }
        else
        {
            addToken(SLASH);
        }
    }
    else if (c == ' ' || c == '\r' || c == '\t')
    {
    }
    else if (c == '\n')
    {
        nextLine();
    }
    else if (c == '"')
    {
        string();
    }
    else if (c == 'o')
    {
        if (match('r'))
        {
            addToken(OR);
        }
    }
    else
    {
        if (isdigit(c))
        {
            number();
        }
        else if (isalpha(c))
        {
            identifier();
        }
        else
        {
            std::cout << c << std::endl;
            ErrorHandler::error("Unexpected character.", line, current);
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
    addToken(type, nullptr);
}

void Scanner::addToken(TokenType type, std::string *literal)
{
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, literal, line, start - charactersAtLineStart));
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
        if (peek() == '\n')
            nextLine();
        advance();
    }

    if (isAtEnd())
    {
        ErrorHandler::error("Unterminated string.", line);
        return;
    }

    advance();

    std::string *value = new std::string(source.substr(start + 1, (current - 1) - (start + 1)));
    addToken(STRING, value);
}

void Scanner::number()
{
    while (isdigit(peek()))
    {
        advance();
    }

    if (peek() == '.' && isdigit(peekNext()))
    {
        advance();
        while (isdigit(peek()))
        {
            advance();
        }
    }

    std::string *value = new std::string(source.substr(start, current - start));
    addToken(NUMBER, value);
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
    while (isalnum(peek()))
    {
        advance();
    }

    std::string text = source.substr(start, current - start);
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

void Scanner::nextLine()
{
    line++;
    charactersAtLineStart = start;
}