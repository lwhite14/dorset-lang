#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include "error.h"
#include "lexer.h"
#include "outpututils.h"
#include "ast.h"
#include "astbuilder.h"

static bool fileExists(std::string fileName);

class CompilerOptions
{
private:
    std::vector<std::string> arguments;

    int currentArgumentIndex = 0;

    bool isHelp = false;
    bool isVersion = false;
    bool isTokens = false;
    bool hasSourceFile = false;
    bool hasOutputName = false;
    bool isLibrary = false;
    bool generateLLVMIR = false;

    bool hadError = false;

    std::string sourceFile;
    std::string sourceFileLocation;

    std::string outputLL;
    std::string outputS;
    std::string outputO;
    std::string outputFinal;

    void advanceArgument();
    std::string currentArgument();
    bool isAtEnd();
    void error(std::string message);

    std::string removeFileExtension(std::string fileName);

    void processFlag();
    void processFile();

    void constructOutputBinaryNames();

public:
    CompilerOptions(int argc, char *argv[]);

    friend class Compiler;
};

class Compiler
{
private:
    CompilerOptions options;

    std::string getSourceContents(std::string fileName);
    std::vector<Token> lex(std::string contents);
    void buildAST(std::vector<Token> tokens);

    void outputBinaries();
    void removeBinaries();

public:
    Compiler(CompilerOptions options);

    int compile();
};
