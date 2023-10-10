#include "cli.h"

#include "outpututils.h"

void CompilerOptions::advanceArgument()
{
    currentArgumentIndex++;
}

std::string CompilerOptions::currentArgument()
{
    return arguments[currentArgumentIndex];
}

bool CompilerOptions::isAtEnd()
{
    if (currentArgumentIndex == arguments.size())
    {
        return true;
    }
    return false;
}

void CompilerOptions::error(std::string message)
{
    std::cout << message << std::endl;
    hadError = true;
}

std::string CompilerOptions::removeFileExtension(std::string fileName)
{
    size_t lastindex = fileName.find_last_of(".");
    return fileName.substr(0, lastindex);
}

bool CompilerOptions::fileExists(std::string fileName)
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

void CompilerOptions::processFlag()
{
    if (currentArgument() == "-v" || currentArgument() == "--version")
    {
        isVersion = true;
    }
    else if (currentArgument() == "-h" || currentArgument() == "--help")
    {
        isHelp = true;
    }
    else if (currentArgument() == "-t" || currentArgument() == "--tokens")
    {
        isTokens = true;
    }
    else
    {
        error("Flag not recognised.");
    }
}

void CompilerOptions::processFile()
{
    if (!fileExists(currentArgument()))
    {
        error("File does not exist.");
        return;
    }

    sourceFile = removeFileExtension(currentArgument());
    sourceFileLocation = std::filesystem::absolute(currentArgument());
    hasSourceFile = true;
}

CompilerOptions::CompilerOptions(int argc, char *argv[])
{
    for (unsigned int i = 1; i < argc; i++)
    {
        arguments.push_back(argv[i]);
    }

    if (arguments.size() == 0)
    {
        error("No arguments.");
        return;
    }

    while (!isAtEnd())
    {
        if (currentArgument()[0] == '-')
        {
            processFlag();
        }
        else
        {
            processFile();
        }

        advanceArgument();
    }

    if (isTokens && !hasSourceFile)
    {
        error("Cannot display tokens without an input source file.");
    }
}

bool CompilerOptions::getIsHelp()
{
    return isHelp;
}

bool CompilerOptions::getIsVersion()
{
    return isVersion;
}

bool CompilerOptions::getIsTokens()
{
    return isTokens;
}

bool CompilerOptions::getHasSourceFile()
{
    return hasSourceFile;
}

std::string CompilerOptions::getSourceFile()
{
    return sourceFile;
}

std::string CompilerOptions::getSourceFileLocation()
{
    return sourceFileLocation;
}

bool CompilerOptions::getHadError()
{
    return hadError;
}