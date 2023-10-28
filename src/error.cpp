#include "error.h"

#include <iomanip>

/////////////////////////
/// Private Functions ///
/////////////////////////

void ErrorHandler::report(std::string message)
{
    std::string first = "\033[31m[unknown]";
    std::string second = "Error: " + message + "\033[0m";

    std::cout << std::left << std::setw(24) << first << std::setw(24) << second << std::endl;
    HadError = true;
}

void ErrorHandler::report(std::string message, int line)
{
    std::string first = "\033[31m[line " + std::to_string(line) + "]";
    std::string second = "Error: " + message + "\033[0m";

    std::cout << std::left  << std::setw(24) << first << std::setw(24) << second << std::endl;
    HadError = true;
}

void ErrorHandler::report(std::string message, int line, int character)
{
    std::string first = "\033[31m[line " + std::to_string(line) + ", char " + std::to_string(character) + "]";
    std::string second = "Error: " + message + "\033[0m";

    std::cout << std::left  << std::setw(24) << first << std::setw(24) << second << std::endl;
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