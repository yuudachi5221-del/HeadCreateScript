#pragma once

#include <string>

enum class TokenType {
    // End of file
    EOF_,
    
    // Identifiers and literals
    Identifier,
    Number,
    String,
    Bool,
    
    // Keywords
    Keyword,  // con, form, const, if, ifel, else, for, name, range, dispin, blank, true, false
    
    // Symbols and operators
    Symbol,  // (, ), {, }, ;, <-, etc.
    
    // Operators
    Plus,       // +
    Minus,      // -
    Star,       // *
    Slash,      // /
    Percent,    // %
    Assign,     // =
    Equal,      // ==
    NotEqual,   // !=
    Less,       // <
    Greater,    // >
    LessEq,     // <=
    GreaterEq,  // >=
    Arrow,      // <-
    
    // Delimiters
    LParen,     // (
    RParen,     // )
    LBrace,     // {
    RBrace,     // }
    Semicolon,  // ;
    
    // Error
    Unknown
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int col;
    
    // Additional fields for error reporting and value storage
    std::string value;  // For string literals, numeric strings, etc.
    bool has_error = false;
    std::string error_message;
    
    Token(TokenType t = TokenType::Unknown, std::string tx = "", int l = 0, int c = 0)
        : type(t), text(std::move(tx)), line(l), col(c), value(text) {}
    
    Token(TokenType t, std::string tx, int l, int c, std::string val)
        : type(t), text(std::move(tx)), line(l), col(c), value(std::move(val)) {}
};
