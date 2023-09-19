#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

#include "error.h"
#include "scanner.h"


std::string getSourceContents(std::string fileName)
{
    std::ifstream file(fileName);
    std::string text;
    std::string output;
    while (getline(file, text))
    {
        output = output + text + '\n';
    }
    return output;
}


std::vector<Token> tokenize(std::string contents)
{
    Scanner* scanner = new Scanner(contents);
    return scanner->scanTokens();
}


void outputTokenInfo(std::vector<Token> tokens)
{
    std::string headingOne = "Token Type";
    std::string headingTwo = "Lexeme";
    std::string headingThree = "Literal";

    std::cout << std::setw(20);
    std::cout << "Token Type";
    std::cout << std::setw(20);
    std::cout << "Lexeme";
    std::cout << std::setw(20);
    std::cout << "Literal";
    std::cout << std::setw(20);
    std::cout << "Line";
    std::cout << std::endl;
    std::cout << "----------------------------------------------";
    std::cout << "----------------------------------------------" << std::endl;

    for (auto token : tokens) 
    {
        std::cout << std::setw(20);
        std::cout << token.getTypeStr();
        std::cout << std::setw(20);
        std::cout << token.getLexeme();
        std::cout << std::setw(20);
        std::cout << token.getLiteral();
        std::cout << std::setw(20);
        std::cout << token.getLine();
        std::cout << std::endl;
    }
}


void printUsage()
{
    std::cout << "Usage: summit [file]" << std::endl;
}


int main(int argc, char* argv[])
{
    if (argc == 2)
    {
        std::vector<Token> tokens = tokenize(getSourceContents(argv[1]));

        outputTokenInfo(tokens);
    }
    else
    {
        printUsage();
    }

    return 0;
}
