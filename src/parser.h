#pragma once

#include <string>
#include <vector>

#include "token.h"
#include "error.h"

class Parser
{
private:
    std::vector<Token> tokens;
    int currentTokenIndex;

    Token currentToken();
    Token advanceToken();

public:
    Parser(std::vector<Token> tokens);
    
};