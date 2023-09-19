#include "error.h"

void ErrorHandler::report(int line, std::string where, std::string message)
{
    std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void ErrorHandler::report(int line, int character, std::string where, std::string message)
{
    std::cout << "[line " << line << ", char " << character << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}

void ErrorHandler::error(int line, std::string message)
{
    report(line, "", message);
}

void ErrorHandler::error(int line, int character, std::string message)
{
    report(line, character, "", message);
}