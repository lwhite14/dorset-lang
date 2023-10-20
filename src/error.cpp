#include "error.h"

/////////////////////////
/// Private Functions ///
/////////////////////////

void ErrorHandler::report(std::string message)
{
    std::cout << "[unknown] Error: " << message << std::endl;
    HadError = true;
}

void ErrorHandler::report(std::string message, int line)
{
    std::cout << "[line " << line << "] Error: " << message << std::endl;
    HadError = true;
}

void ErrorHandler::report(std::string message, int line, int character)
{
    std::cout << "[line " << line << ", char " << character << "] Error: " << message << std::endl;
    HadError = true;
}

////////////////////////
/// Public Functions ///
////////////////////////

void ErrorHandler::error(std::string message)
{
    report(message);
}

void ErrorHandler::error(std::string message, int line)
{
    report(message, line);
}

void ErrorHandler::error(std::string message, int line, int character)
{
    report(message, line, character);
}