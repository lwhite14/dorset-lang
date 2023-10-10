#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

class CompilerOptions
{
private:
    std::vector<std::string> arguments;

    int currentArgumentIndex = 0;

    bool isHelp = false;
    bool isVersion = false;
    bool isTokens = false;
    bool hasSourceFile = false;

    bool hadError = false;

    std::string sourceFile;
    std::string sourceFileLocation;

    void advanceArgument();
    std::string currentArgument();
    bool isAtEnd();
    void error(std::string message);

    std::string removeFileExtension(std::string fileName);
    bool fileExists(std::string fileName);


public:
    CompilerOptions(int argc, char *argv[]);

    bool getIsHelp();
    bool getIsVersion();
    bool getIsTokens();
    bool getHasSourceFile();
    std::string getSourceFile();
    std::string getSourceFileLocation();
    bool getHadError();
};

