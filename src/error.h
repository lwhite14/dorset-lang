#pragma once

#include <string>
#include <iostream>

class ErrorHandler
{
private:
    static void report(int line, std::string where, std::string message);
    static void report(int line, int character, std::string where, std::string message);

public:
    inline static bool HadError = false;

    static void error(int line, std::string message);
    static void error(int line, int character, std::string message);
};