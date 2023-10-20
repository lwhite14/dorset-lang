#include "cli.h"

#include "outpututils.h"

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

/////////////////////////
//// CompilerOptions ////
/////////////////////////

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
    SourceFileLocation = std::filesystem::absolute(currentArgument()).generic_string();
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

/////////////////////////
/////// Compiler ////////
/////////////////////////

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

        if (!ErrorHandler::HadError)
        {
            outputBinaries();
            removeBinaries();
        }
    }
    else
    {
        printUsage();
    }

    return 0;
}

void Compiler::outputBinaries()
{
    std::string ir;
    raw_string_ostream ostream(ir);
    ostream << *AST::MasterAST::TheModule;
    ostream.flush();

    std::ofstream irFile;
    irFile.open(CompilerOptions::OutputLL);
    irFile << ir;
    irFile.close();

    if (system(("llc " + CompilerOptions::OutputLL + " -o " + CompilerOptions::OutputS).c_str()) != 0)
    {
        ErrorHandler::error("error compiling LLVM IR");
        return;
    }
    if (system(("clang -c " + CompilerOptions::OutputS + " -o " + CompilerOptions::OutputO).c_str()) != 0)
    {
        ErrorHandler::error("error compiling assembly");
        return;
    }
    if (!CompilerOptions::IsLibrary)
    {
        if (system(("clang " + CompilerOptions::OutputO + " -o " + CompilerOptions::OutputFinal + " -no-pie").c_str()) != 0)
        {
            ErrorHandler::error("error compiling object file");
            return;
        }
    }
}

void Compiler::removeBinaries()
{
    if (!CompilerOptions::IsLibrary || ErrorHandler::HadError)
    {
        if (fileExists(CompilerOptions::OutputO))
        {
            system(("rm " + CompilerOptions::OutputO).c_str());
        }
    }
    if (!CompilerOptions::GenerateLLVMIR || ErrorHandler::HadError)
    {
        if (fileExists(CompilerOptions::OutputLL))
        {
            system(("rm " + CompilerOptions::OutputLL).c_str());
        }
    }
    if (fileExists(CompilerOptions::OutputS))
    {
        system(("rm " + CompilerOptions::OutputS).c_str());
    }
}