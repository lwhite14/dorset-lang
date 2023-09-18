#include <string>
#include <iostream>

class ErrorHandler
{
private:
    inline static bool hadError = false;

    static void report(int line, std::string where, std::string message);

public:
    static void error(int line, std::string message);
};