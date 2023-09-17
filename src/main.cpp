#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum Token
{
    // Special
    EndOfFile,
    EndOfLine,
    BlockStart,
    BlockEnd,
    _Return,
    SemiColon,

    // Others
    Function,
    Identifier,
    Type,
    Number
};

static std::vector<Token> tokens;

std::string GetSourceContents(std::string fileName)
{
    std::ifstream file(fileName);
    std::string text;
    std::string output;
    while (getline(file, text))
     {
        output += text;
        output += " ";
    }
    return output;
}

void PushBackToken(std::string currentToken)
{
    if (currentToken == "fn")
    {
        tokens.push_back(Token::Function);
    }         
    if (currentToken == "main")
    {
        tokens.push_back(Token::Identifier);
    }      
    if (currentToken == ";")
    {
        tokens.push_back(Token::SemiColon);
    }
    if (currentToken == "{")
    {
        tokens.push_back(Token::BlockStart);
    }
    if (currentToken == "}")
    {
        tokens.push_back(Token::BlockEnd);
    }
    if (currentToken == "integer")
    {
        tokens.push_back(Token::Type);
    }
    if (currentToken == "return")
    {
        tokens.push_back(Token::_Return);
    }
}


void Tokenize(std::string contents)
{
    std::string currentToken;
    for (int i = 0; i < contents.size(); i++)
    {
        if (!isspace(contents[i]))
        {
            currentToken += contents[i];
        }
        else
        {
            PushBackToken(currentToken);
            currentToken.clear();
        }
    }

}


std::string TokenEnumToString(int tokenEnum)
{
    if (tokenEnum == 0) { return "EndOfFile"; }
    if (tokenEnum == 1) { return "EndOfLine"; }
    if (tokenEnum == 2) { return "BlockStart"; }
    if (tokenEnum == 3) { return "BlockEnd"; }
    if (tokenEnum == 4) { return "Return"; }
    if (tokenEnum == 5) { return "SemiColon"; }
    if (tokenEnum == 6) { return "Function"; }
    if (tokenEnum == 7) { return "Identifier"; }
    if (tokenEnum == 8) { return "Type"; }
    if (tokenEnum == 9) { return "Number"; }
    return "Unknown";
}


int main(int argc, char* argv[])
{
    std::string fileContents;
    if (argc > 1)
    {
        fileContents = GetSourceContents(argv[1]);
    }

    Tokenize(fileContents);

    std::cout << fileContents << std::endl;

    for (auto n : tokens)
    {
        std::cout << TokenEnumToString(n) << std::endl;
    }

    return 0;
}
