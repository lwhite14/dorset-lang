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

CompilerOptions::CompilerOptions(int argc, char *argv[])
{
    for (unsigned int i = 1; i < argc; i++)
    {
        arguments.push_back(argv[i]);
    }

    while (!isAtEnd())
    {
        if (currentArgument()[0] == '-')
        {
            if (currentArgument() == "-v")
            {
                isVersion = true;
            }
            else if (currentArgument() == "-h")
            {
                isHelp = true;
            }
            else if (currentArgument() == "-t")
            {
                isTokens = true;
            }
            else
            {
                std::cout << "Flag not recognised." << std::endl;
                hadError = true;
            }
        }
        else
        {
            if (fileExists(currentArgument()))
            {
                sourceFile = removeFileExtension(currentArgument());
                sourceFileLocation = std::filesystem::absolute(currentArgument());
                hasSourceFile = true;
            }
            else
            {
                std::cout << "File does not exist." << std::endl;
                hadError = true;
            }
        }

        advanceArgument();
    }

    if (isTokens && !hasSourceFile)
    {
        std::cout << "Cannot display tokens with input source file." << std::endl;
        hadError = true;
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