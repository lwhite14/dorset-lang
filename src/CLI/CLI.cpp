#include "CLI.h"

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
    if (currentArgumentIndex >= arguments.size())
    {
        return "";
    }
    return arguments[currentArgumentIndex];
}

bool CompilerOptions::isAtEnd()
{
    if (currentArgumentIndex >= arguments.size())
    {
        return true;
    }
    return false;
}

void CompilerOptions::error(std::string message)
{
    std::cout << "\033[31m" << message << "\033[0m" << std::endl;
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
        if (currentArgument() == "")
        {
            error("No argument given to output flag.");
            return;
        }
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
    else if (currentArgument() == "-b" || currentArgument() == "--keepbin")
    {
        deleteBinaries = false;
    }
    else if (currentArgument() == "-rs")
    {
        hasRawCode = true;
        advanceArgument();
        rawCode = currentArgument();
    }
    else
    {
        error("Flag not recognised.");
    }
}

void CompilerOptions::processFile()
{
    if (!hasSourceFile && !hasRawCode) // only read the first source file (only one source supported 0.1.4)
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

CompilerOptions::CompilerOptions(std::vector<std::string> args) // For testing purproses
{
    for (unsigned int i = 0; i < args.size(); i++)
    {
        arguments.push_back(args[i]);
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

bool CompilerOptions::getHadError() 
{
    return hadError;
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
        return 1;
    }

    if (options.isHelp)
    {
        printUsage();
    }
    else if (options.isVersion)
    {
        printVersion();
    }
    else if (options.hasSourceFile || options.hasRawCode)
    {
        std::vector<Token> tokens;
        if (options.hasRawCode) 
        {
            tokens = lex(options.rawCode);
        }
        else 
        {
            tokens = lex(getSourceContents(options.sourceFileLocation));
        }


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
            if (options.deleteBinaries) 
            {
                removeBinaries();
            }
        }
    }
    else
    {
        printUsage();
    }

    if (ErrorHandler::HadError)
        return 1;

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
#if defined(_WIN64) || defined(_WIN32)
        if (system(("clang " + options.outputS + " -o " + options.outputFinal).c_str()) != 0)
#else
        if (system(("clang " + options.outputS + " -o " + options.outputFinal + " -no-pie").c_str()) != 0)
#endif
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