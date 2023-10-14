#include "outpututils.h"

#include <iostream>
#include <iomanip>

#include "version.h"

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "?"
#endif

#ifndef GIT_BRANCH
#define GIT_BRANCH "?"
#endif

void printUsage()
{
    std::cout << "Usage: dorset [file] [options]                    " << std::endl;
    std::cout << "                                                  " << std::endl;
    std::cout << "Options:                                          " << std::endl;
    std::cout << "    -t --tokens   = list all the tokens           " << std::endl;
    std::cout << "    -h --help     = print the usage               " << std::endl;
    std::cout << "    -v --version  = print the version             " << std::endl;
    std::cout << "    -o <filename> = specify the output name       " << std::endl;
    std::cout << "    -l --library  = generate a library file       " << std::endl;
    std::cout << "    -r --llvmir   = output LLVM IR file           " << std::endl;
    std::cout << "                                                  " << std::endl;
}

void printVersion()
{
    std::cout << "dorset-lang " << Version.to_string() << std::endl;
    std::cout << "(" << GIT_COMMIT_HASH << ")" << std::endl;
}

void printTokens(std::vector<Token> tokens)
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