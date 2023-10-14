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
    else if (currentArgument() == "-o")
    {
        advanceArgument();
        OutputFinal = currentArgument();
        hasOutputName = true;
    }
    else if (currentArgument() == "-l" || currentArgument() == "--library")
    {
        IsLibrary = true;
    }
    else if (currentArgument() == "-r" || currentArgument() == "--llvmir")
    {
        GenerateLLVMIR = true;
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

    SourceFile = removeFileExtension(currentArgument());
    SourceFileLocation = std::filesystem::absolute(currentArgument());
    hasSourceFile = true;
}

void CompilerOptions::constructOutputBinaryNames()
{
    if (hasOutputName)
    {
        std::string name = removeFileExtension(OutputFinal);
        OutputLL = name + ".ll";
        OutputO = name + ".o";
        OutputS = name + ".s";
    }
    else
    {
        OutputFinal = SourceFile + ".out";
        OutputLL = SourceFile + ".ll";
        OutputO = SourceFile + ".o";
        OutputS = SourceFile + ".s";
    }
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

    constructOutputBinaryNames();

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

bool CompilerOptions::getHadError()
{
    return hadError;
}

bool CompilerOptions::getHasOutputName()
{
    return hasOutputName;
}

bool CompilerOptions::getIsLibrary()
{
    return IsLibrary;
}


//////////////////
//// Compiler ////
//////////////////

std::string Compiler::getSourceContents(std::string fileName)
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

std::vector<Token> Compiler::lex(std::string contents)
{
    Lexer lexer = Lexer(contents);
    return lexer.scanTokens();
}

void Compiler::buildAST(std::vector<Token> tokens)
{
    ASTBuilder parser = ASTBuilder(tokens);
    parser.parseTokenList();
}

Compiler::Compiler(CompilerOptions options) : options{options}
{
}

int Compiler::compile()
{
    if (options.getHadError())
    {
        printUsage();
        return 0;
    }

    if (options.getIsHelp())
    {
        printUsage();
    }
    else if (options.getIsVersion())
    {
        printVersion();
    }
    else if (options.getHasSourceFile())
    {
        std::vector<Token> tokens = lex(getSourceContents(CompilerOptions::SourceFileLocation));
        if (options.getIsTokens())
        {
            printTokens(tokens);
        }
        buildAST(tokens);
    }
    else
    {
        printUsage();
    }

    return 0;
}
