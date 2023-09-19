#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "error.h"
#include "scanner.h"
#include "outpututils.h"


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
    std::unique_ptr<Scanner> scanner = std::unique_ptr<Scanner>(new Scanner(contents));
    return scanner->scanTokens();
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
