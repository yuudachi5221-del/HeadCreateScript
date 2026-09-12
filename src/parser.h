#pragma once

#include <vector>
#include "token.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    // Parse method to be implemented in future
    ProgramNode parse();

private:
    const std::vector<Token>& tokens_;
    size_t pos_ = 0;
};
