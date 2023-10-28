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
            // removeBinaries();
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
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    std::string TargetTriple = sys::getDefaultTargetTriple();
    AST::MasterAST::TheModule->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    if (!Target)
    {
        ErrorHandler::error("could not create target");
        exit(1);
    }

    auto TheTargetMachine = Target->createTargetMachine(TargetTriple, "generic", "", TargetOptions(), std::optional<Reloc::Model>());

    AST::MasterAST::TheModule->setDataLayout(TheTargetMachine->createDataLayout());

    StringRef fileName = options.outputO;
    std::error_code EC;
    raw_fd_ostream dest(fileName, EC, sys::fs::OF_None);

    if (EC)
    {
        ErrorHandler::error("could not open file: " + fileName.str());
        exit(1);
    }

    legacy::PassManager pass;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, CodeGenFileType::ObjectFile))
    {
        ErrorHandler::error("can't emit a file of this type");
        exit(1);
    }

    pass.run(*AST::MasterAST::TheModule);
    dest.flush();
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