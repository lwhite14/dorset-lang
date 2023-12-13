#include <dorsetDriver/LexicalAnalysis/Lexer.h>

#include <ctype.h>

Lexer::Lexer(std::string source)
{
    this->source = source;
}

std::vector<Token> Lexer::scanTokens()
{
    while (!isAtEnd())
    {
        start = current;
        scanToken();
    }

    tokens.push_back(Token(_EOF, "", "", line, start - charactersAtLineStart));
    return tokens;
}

void Lexer::scanToken()
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
    else if (c == '|')
    {
        addToken(VERTICAL_BAR);
    }
    else if (c == '&')
    {
        addToken(AMPERSAND);
    }
    else if (c == '^')
    {
        addToken(CARET);
    }
    else if (c == ':')
    {
        addToken(COLON);
    }
    else if (c == '\\')
    {
        addToken(BACK_SLASH);
    }
    else if (c == '[') 
    {
        addToken(LEFT_SQUARE);
    }
    else if (c == ']') 
    {
        addToken(RIGHT_SQUARE);
    }
    else if (c == '!')
    {
        addToken(match('=') ? EXCLAMATION_EQUAL : EXCLAMATION);
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
            addToken(FORWARD_SLASH);
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
            nonKeywordAlpha();
        }
        else
        {
            std::string errorMessage = "unexpected character: ";
            errorMessage.push_back(c);
            ErrorHandler::error(errorMessage.c_str(), line, current);
        }
    }
}

bool Lexer::isAtEnd()
{
    return current >= source.size();
}

char Lexer::advance()
{
    return source[current++];
}

void Lexer::addToken(enum TokenType type)
{
    addToken(type, "");
}

void Lexer::addToken(enum TokenType type, std::string literal)
{
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, text, literal, line, start - charactersAtLineStart));
}

bool Lexer::match(char expected)
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

char Lexer::peek()
{
    if (isAtEnd())
    {
        return '\0';
    }

    return source[current];
}

void Lexer::string()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n')
            nextLine();
        advance();
    }

    if (isAtEnd())
    {
        ErrorHandler::error("unterminated string", line);
        return;
    }

    advance();

    std::string value = source.substr(start + 1, (current - 1) - (start + 1));
    addToken(STRING, value);
}

void Lexer::number()
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

    std::string value = source.substr(start, current - start);
    addToken(NUMBER, value);
}

char Lexer::peekNext()
{
    if (current + 1 >= source.length())
    {
        return '\0';
    }
    return source[current + 1];
}

void Lexer::nonKeywordAlpha()
{
    while (isalnum(peek()))
    {
        advance();
    }
    std::string text = source.substr(start, current - start);

    if (identifier(text)) { return; }
    if (type(text)) { return; }

    TokenType tokenType = IDENTIFIER;
    addToken(tokenType);
    return;
}

bool Lexer::identifier(std::string text)
{
    TokenType tokenType;
    std::map<std::string, TokenType>::const_iterator pos = keywords.find(text);
    if (pos == keywords.end())
    {
        return false;
    }
    else
    {
        tokenType = pos->second;
        addToken(tokenType);
        return true;
    }
}

bool Lexer::type(std::string text) 
{
    TokenType tokenType;
    std::map<std::string, TokenType>::const_iterator pos = types.find(text);
    if (pos == types.end())
    {
        return false;
    }
    else
    {
        tokenType = pos->second;
        addToken(tokenType);
        return true;
    }
}

void Lexer::nextLine()
{
    line++;
    charactersAtLineStart = start;
}