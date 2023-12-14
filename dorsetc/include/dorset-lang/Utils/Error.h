#pragma once

#include <string>
#include <iostream>

namespace Dorset
{
    class ErrorHandler
    {
    private:
        static void report(std::string message);
        static void report(std::string message, int line);
        static void report(std::string message, int line, int character);

        static void reportWarning(std::string message);
        static void reportWarning(std::string message, int line);
        static void reportWarning(std::string message, int line, int character);

    public:
        inline static bool HadError = false;

        static void error(std::string message);
        static void error(std::string message, int line);
        static void error(std::string message, int line, int character);

        static void warning(std::string message);
        static void warning(std::string message, int line);
        static void warning(std::string message, int line, int character);
    };
}