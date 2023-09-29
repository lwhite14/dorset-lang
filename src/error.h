#pragma once

#include <string>
#include <iostream>

class ErrorHandler
{
private:
    static void report(std::string where, std::string message);
    static void report(std::string where, std::string message, int line);
    static void report(std::string where, std::string message, int line, int character);

public:
    inline static bool HadError = false;

    static void error(std::string message);
    static void error(std::string message, int line);
    static void error(std::string message, int line, int character);
};