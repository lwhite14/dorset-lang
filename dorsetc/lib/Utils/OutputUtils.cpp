#include <dorset-lang/Utils/OutputUtils.h>

#include <iostream>
#include <iomanip>

#include <dorset-lang/Utils/Version.h>

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "?"
#endif

#ifndef GIT_COMMIT_DATE
#define GIT_COMMIT_DATE "?"
#endif

#ifndef GIT_BRANCH
#define GIT_BRANCH "?"
#endif

namespace Dorset
{
    void printUsage()
    {
        std::cout << "Usage: dorsetc [file] <options, ...>             " << std::endl;
        std::cout << "                                                " << std::endl;
        std::cout << "Options:                                        " << std::endl;
        std::cout << "    -t  --tokens   = list all the tokens        " << std::endl;
        std::cout << "    -h  --help     = print the usage            " << std::endl;
        std::cout << "    -v  --version  = print the version          " << std::endl;
        std::cout << "    -o  <filename> = specify the output name    " << std::endl;
        // std::cout << "    -l  --library  = generate a library file    " << std::endl;
        std::cout << "    -r  --llvmir   = output LLVM IR file        " << std::endl;
        std::cout << "    -b  --keepbin  = retain the build binaries  " << std::endl;
        std::cout << "    -rs <code>     = input the raw source       " << std::endl;
        std::cout << "                                                " << std::endl;
    }

    void printVersion()
    {
        std::cout << "dorsetc " << getVersion().to_string() << " ";

        std::string hash = GIT_COMMIT_HASH;
        hash = hash.substr(0, 9);

        std::string date = GIT_COMMIT_DATE;
        date = date.substr(0, 10);

        if (hash == "")
        {
            std::cout << "(?) ";
        }
        else
        {
            std::cout << "(" << hash << ") ";
        }

        if (date == "")
        {
            std::cout << "(?)" << std::endl;
        }
        else
        {
            std::cout << "(" << date << ")" << std::endl;
        }

        std::string branch = GIT_BRANCH;
        if (branch == "develop")
        {
            std::cout << "Built from the development branch, expect bugs.   " << std::endl;
            std::cout << "\033[33mYou have been warned.\033[0m            " << std::endl;
        }
    }

    void printTokens(std::vector<Token> tokens)
    {
        std::cout << std::left << std::setw(24);
        std::cout << "Token Type";
        std::cout << std::setw(24);
        std::cout << "Lexeme";
        std::cout << std::setw(24);
        std::cout << "Literal";
        std::cout << std::setw(24);
        std::cout << "Line";
        std::cout << std::endl;
        std::cout << "----------------------------------------------";
        std::cout << "----------------------------------------------" << std::endl;

        for (auto token : tokens)
        {
            std::cout << std::setw(24);
            std::cout << token.getTypeStr();
            std::cout << std::setw(24);
            std::cout << token.getLexeme();
            std::cout << std::setw(24);
            std::cout << token.getLiteral();
            std::cout << std::setw(24);
            std::cout << token.getLine();
            std::cout << std::endl;
        }
        std::cout << "----------------------------------------------";
        std::cout << "----------------------------------------------" << std::endl;
    }
}