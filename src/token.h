#pragma once

#include <string>

enum class TokenType { EOF_, Identifier, Keyword, Symbol, Number, Bool, Unknown };

struct Token {
    TokenType type;
    std::string text;
    int line;
    int col;
    Token(TokenType t = TokenType::Unknown, std::string tx = "", int l = 0, int c = 0)
        : type(t), text(std::move(tx)), line(l), col(c) {}
};
