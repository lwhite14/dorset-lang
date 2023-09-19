#pragma once

#include <string>
#include <iostream>

class ErrorHandler
{
private:
    inline static bool hadError = false;

    static void report(int line, std::string where, std::string message);
    static void report(int line, int character, std::string where, std::string message);

public:
    static void error(int line, std::string message);
    static void error(int line, int character, std::string message);
};