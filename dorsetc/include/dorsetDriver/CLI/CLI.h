#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>

#include <dorsetDriver/LexicalAnalysis/Lexer.h>
#include <dorsetDriver/Utils/Error.h>
#include <dorsetDriver/Utils/OutputUtils.h>
#include <dorsetDriver/AST/AST.h>
#include <dorsetDriver/Builder/ASTBuilder.h>

static bool fileExists(std::string fileName);

class CompilerOptions
{
private:
    std::vector<std::string> arguments;

    int currentArgumentIndex = 0;

    bool hasSourceFile = false;
    bool hasRawCode = false;

    bool isHelp = false;
    bool isVersion = false;
    bool isTokens = false;
    bool hasOutputName = false;
    bool isLibrary = false;
    bool generateLLVMIR = false;
    bool deleteBinaries = true;

    bool hadError = false;

    std::string sourceFile = "output";
    std::string sourceFileLocation = "output";

    std::string outputLL;
    std::string outputS;
    std::string outputO;
    std::string outputFinal;

    std::string rawCode = "";

    void advanceArgument();
    std::string currentArgument();
    bool isAtEnd();
    void error(std::string message);

    std::string removeFileExtension(std::string fileName);
    std::string removeForwardSlashes(std::string filePath);
    std::string removeBackSlashes(std::string filePath);

    void processFlag();
    void processFile();

    void constructOutputBinaryNames();

public:
    CompilerOptions(int argc, char *argv[]);
    CompilerOptions(std::vector<std::string> args); // For testing purproses

    bool getHadError();

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
