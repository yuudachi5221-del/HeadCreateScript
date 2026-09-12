#pragma once

#include <string>
#include <vector>
#include "token.h"

class Lexer {
public:
    explicit Lexer(const std::string& filename);
    explicit Lexer(std::string_view input, bool fromString);

    std::vector<Token> tokenize();

private:
    std::string input_;
    size_t pos_ = 0;
    int line_ = 1;
    int col_ = 1;

    void initFromFile(const std::string& filename);
    void removeComments();

    char peek(size_t offset = 0) const;
    char get();
    void skipWhitespace();
    bool startsWith(const std::string& s) const;

    static bool isIdentStart(char c);
    static bool isIdentChar(char c);
};
