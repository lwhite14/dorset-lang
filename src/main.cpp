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
#include "cli.h"

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

std::vector<Token> lex(std::string contents)
{
    std::unique_ptr<Lexer> lexer = std::unique_ptr<Lexer>(new Lexer(contents));
    return lexer->scanTokens();
}

void buildAST(std::vector<Token> tokens, std::string fileName, std::string filePath)
{
    std::unique_ptr<ASTBuilder> parser = std::unique_ptr<ASTBuilder>(new ASTBuilder(tokens));
    parser->parseTokenList(fileName, filePath);
}

int main(int argc, char *argv[])
{
    CompilerOptions options = CompilerOptions(argc, argv);

    if (!options.getHadError())
    {
        if (options.getIsHelp())
        {
            printUsage();
        }
        if (options.getIsVersion())
        {
            printVersion();
        }
        if (options.getHasSourceFile())
        {
            std::vector<Token> tokens = lex(getSourceContents(options.getSourceFileLocation()));
            if (options.getIsTokens()) { printTokens(tokens); }
            buildAST(tokens, options.getSourceFile(), options.getSourceFileLocation());
        }
    }
    else
    {
        printUsage();
    }

    return 0;
}
