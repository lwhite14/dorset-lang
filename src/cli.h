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
#include "astbuilder.h"

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

    bool hadError = false;

    void advanceArgument();
    std::string currentArgument();
    bool isAtEnd();
    void error(std::string message);

    std::string removeFileExtension(std::string fileName);
    bool fileExists(std::string fileName);

    void processFlag();
    void processFile();

    void constructOutputBinaryNames();

public:
    static inline std::string SourceFile;
    static inline std::string SourceFileLocation;

    static inline std::string OutputLL;
    static inline std::string OutputS;
    static inline std::string OutputO;
    static inline std::string OutputFinal;

    static inline bool IsLibrary = false;
    static inline bool GenerateLLVMIR = false;

    CompilerOptions(int argc, char *argv[]);

    bool getIsHelp();
    bool getIsVersion();
    bool getIsTokens();
    bool getHasSourceFile();
    bool getHadError();
    bool getHasOutputName();
    bool getIsLibrary();
};

class Compiler
{
private:
    CompilerOptions options;

    std::string getSourceContents(std::string fileName);
    std::vector<Token> lex(std::string contents);
    void buildAST(std::vector<Token> tokens);

public:
    Compiler(CompilerOptions options);

    int compile();

};
