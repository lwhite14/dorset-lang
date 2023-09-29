#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include "error.h"
#include "lexer.h"
#include "outpututils.h"
#include "astbuilder.h"

bool fileExists(std::string fileName)
{
    if (FILE *file = fopen(fileName.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

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
    std::unique_ptr<Lexer> lexer = std::unique_ptr<Lexer>(new Lexer(contents));
    return lexer->scanTokens();
}

void parserize(std::vector<Token> tokens, std::string fileName, std::string filePath)
{
    std::unique_ptr<ASTBuilder> parser = std::unique_ptr<ASTBuilder>(new ASTBuilder(tokens));
    parser->parseTokenList(fileName, filePath);
}

std::string removeFileExtension(std::string fileName)
{
    size_t lastindex = fileName.find_last_of(".");
    return fileName.substr(0, lastindex);
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        if (fileExists(argv[1]))
        {
            std::vector<Token> tokens = tokenize(getSourceContents(argv[1]));
            outputTokenInfo(tokens);
            parserize(tokens, removeFileExtension(argv[1]), std::filesystem::absolute(argv[1]));
        }
        else
        {
            std::cout << "File does not exist." << std::endl;
            printUsage();
        }
    }
    else
    {
        printUsage();
    }

    return 0;
}
