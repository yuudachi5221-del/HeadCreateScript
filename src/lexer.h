#pragma once

#include <string>
#include <vector>
#include <memory>
#include "token.h"

class Lexer {
public:
    // Constructor from file
    explicit Lexer(const std::string& filename);
    
    // Constructor from string input
    explicit Lexer(std::string_view input, bool fromString = true);
    
    // Main tokenization function
    std::vector<Token> tokenize();
    
    // Get current error state
    bool hasError() const { return error_state_; }
    const std::string& getErrorMessage() const { return error_message_; }
    
    // Get source code for error reporting
    const std::string& getSource() const { return input_; }
    
private:
    std::string input_;
    size_t pos_ = 0;
    int line_ = 1;
    int col_ = 1;
    
    // Error tracking
    bool error_state_ = false;
    std::string error_message_;
    
    // File loading
    void initFromFile(const std::string& filename);
    
    // Comment and whitespace handling
    void removeComments();
    void skipWhitespace();
    
    // Character inspection
    char peek(size_t offset = 0) const;
    char get();
    bool startsWith(const std::string& s) const;
    
    // Character classification
    static bool isIdentStart(char c);
    static bool isIdentChar(char c);
    static bool isDigit(char c);
    static bool isOctalDigit(char c);
    static bool isHexDigit(char c);
    static bool isWhitespace(char c);
    
    // Token recognition helpers
    Token recognizeKeywordOrIdentifier();
    Token recognizeNumber();
    Token recognizeString();
    Token recognizeOperatorOrSymbol();
    
    // Error handling
    void setError(const std::string& message);
    Token createErrorToken(const std::string& message);
    
    // Helper for getting current position info
    int getCurrentLine() const { return line_; }
    int getCurrentCol() const { return col_; }
};
