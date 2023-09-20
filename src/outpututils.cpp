#include "outpututils.h"

#include <iostream>
#include <iomanip>

void printUsage()
{
    std::cout << "Usage: summit [file]" << std::endl;
}

void outputTokenInfo(std::vector<Token> tokens)
{
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
    std::cout << "----------------------------------------------";
    std::cout << "----------------------------------------------" << std::endl;
}