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
        outputFinal = currentArgument();
        hasOutputName = true;
    }
    else if (currentArgument() == "-l" || currentArgument() == "--library")
    {
        isLibrary = true;
    }
    else if (currentArgument() == "-r" || currentArgument() == "--llvmir")
    {
        generateLLVMIR = true;
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
    sourceFileLocation = std::filesystem::absolute(currentArgument()).generic_string();
    hasSourceFile = true;
}

void CompilerOptions::constructOutputBinaryNames()
{
    if (hasOutputName)
    {
        std::string name = removeFileExtension(outputFinal);
        outputLL = name + ".ll";
        outputO = name + ".o";
        outputS = name + ".s";
    }
    else
    {
        outputFinal = sourceFile + ".out";
        outputLL = sourceFile + ".ll";
        outputO = sourceFile + ".o";
        outputS = sourceFile + ".s";
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
    if (options.hadError)
    {
        printUsage();
        return 0;
    }

    if (options.isHelp)
    {
        printUsage();
    }
    else if (options.isVersion)
    {
        printVersion();
    }
    else if (options.hasSourceFile)
    {
        std::vector<Token> tokens = lex(getSourceContents(options.sourceFileLocation));
        if (options.isTokens)
        {
            printTokens(tokens);
        }
        AST::MasterAST::initializeModule(options.sourceFileLocation.c_str());
        AST::createExternalFunctions();
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
    irFile.open(options.outputLL);
    irFile << ir;
    irFile.close();

    if (system(("llc " + options.outputLL + " -o " + options.outputS).c_str()) != 0)
    {
        ErrorHandler::error("error compiling LLVM IR");
        return;
    }
    if (system(("clang -c " + options.outputS + " -o " + options.outputO).c_str()) != 0)
    {
        ErrorHandler::error("error compiling assembly");
        return;
    }
    if (!options.isLibrary)
    {
        if (system(("clang " + options.outputS + " -o " + options.outputFinal + " -no-pie").c_str()) != 0)
        {
            ErrorHandler::error("error compiling object file");
            return;
        }
    }
}

void Compiler::removeBinaries()
{
    if (!options.isLibrary || ErrorHandler::HadError)
    {
        if (fileExists(options.outputO))
        {
            system(("rm " + options.outputO).c_str());
        }
    }
    if (!options.generateLLVMIR || ErrorHandler::HadError)
    {
        if (fileExists(options.outputLL))
        {
            system(("rm " + options.outputLL).c_str());
        }
    }
    if (fileExists(options.outputS))
    {
        system(("rm " + options.outputS).c_str());
    }
}