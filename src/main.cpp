#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "error.h"
#include "scanner.h"

std::string getSourceContents(std::string fileName)
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


void tokenize(std::string contents)
{
    Scanner* scanner = new Scanner(contents);
    std::vector<Token> tokens = scanner->scanTokens();

    for (auto token : tokens) 
    {
        std::cout << token.toString() << std::endl;
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
        std::string fileContents = getSourceContents(argv[1]);

        tokenize(fileContents);

        std::cout << "\n" << fileContents << std::endl;
    }
    else
    {
        printUsage();
    }

    return 0;
}
