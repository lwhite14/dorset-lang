#pragma once

#include <vector>

#include <dorset-lang/LexicalAnalysis/Token.h>

namespace Dorset
{
    void printUsage();
    void printVersion();
    void printTokens(std::vector<Token> tokens);
}