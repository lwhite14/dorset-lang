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
    std::string headingThree = "Line";

    std::cout << headingOne;
    std::cout << std::setw(30 - headingOne.length());
    std::cout << headingTwo;
    std::cout << std::setw(15);
    std::cout << headingThree;
    std::cout << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    for (auto token : tokens) 
    {
        std::cout << token.getTypeStr();
        std::cout << std::setw(30 - token.getTypeStr().length());
        std::cout << token.getLexeme();
        std::cout << std::setw(15);
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
