#pragma once

#include <string>
#include <vector>
#include "token.h"

struct ErrorLocation {
    int line = 0;
    int column = 0;
    std::string filename;
    
    ErrorLocation() = default;
    ErrorLocation(int l, int c, const std::string& f = "")
        : line(l), column(c), filename(f) {}
};

struct LexError {
    ErrorLocation location;
    std::string message;
    std::string context;  // The line of source code where error occurred
    std::string hint;     // Helpful hint for fixing the error
    
    LexError() = default;
    LexError(const ErrorLocation& loc, const std::string& msg)
        : location(loc), message(msg) {}
    
    std::string format() const {
        std::string result;
        if (!location.filename.empty()) {
            result += location.filename + ":";
        }
        result += std::to_string(location.line) + ":" + std::to_string(location.column);
        result += ": Error: " + message;
        
        if (!context.empty()) {
            result += "\n" + context + "\n";
            // Add caret pointer
            for (int i = 0; i < location.column - 1; ++i) {
                result += " ";
            }
            result += "^";
        }
        
        if (!hint.empty()) {
            result += "\nHint: " + hint;
        }
        
        return result;
    }
};

class LexerErrorHandler {
public:
    void addError(const LexError& error) {
        errors_.push_back(error);
    }
    
    void addError(const ErrorLocation& loc, const std::string& message) {
        errors_.emplace_back(loc, message);
    }
    
    void addError(int line, int col, const std::string& message, 
                  const std::string& filename = "") {
        errors_.emplace_back(ErrorLocation(line, col, filename), message);
    }
    
    bool hasErrors() const {
        return !errors_.empty();
    }
    
    size_t errorCount() const {
        return errors_.size();
    }
    
    const std::vector<LexError>& getErrors() const {
        return errors_;
    }
    
    void clear() {
        errors_.clear();
    }
    
    std::string formatAll() const {
        std::string result;
        for (const auto& error : errors_) {
            result += error.format() + "\n";
        }
        return result;
    }
    
    // Specific error types with helpful hints
    void reportUnterminatedString(int line, int col, const std::string& filename = "") {
        LexError err(ErrorLocation(line, col, filename), "Unterminated string literal");
        err.hint = "String literals must be closed with a matching double quote (\")";
        addError(err);
    }
    
    void reportUnterminatedComment(int line, int col, const std::string& filename = "") {
        LexError err(ErrorLocation(line, col, filename), "Unterminated comment");
        err.hint = "Comments must be closed with /> (e.g., </ comment />)";
        addError(err);
    }
    
    void reportInvalidNumber(int line, int col, const std::string& number, 
                            const std::string& filename = "") {
        LexError err(ErrorLocation(line, col, filename), 
                    "Invalid number literal: " + number);
        err.hint = "Numbers must be valid decimal, octal (0...), hex (0x...), or scientific notation";
        addError(err);
    }
    
    void reportUnknownCharacter(int line, int col, char c, const std::string& filename = "") {
        LexError err(ErrorLocation(line, col, filename), 
                    std::string("Unknown character: '") + c + "'");
        err.hint = "This character is not recognized in HCS syntax";
        addError(err);
    }
    
    void reportInvalidEscapeSequence(int line, int col, char seq, 
                                     const std::string& filename = "") {
        LexError err(ErrorLocation(line, col, filename), 
                    std::string("Invalid escape sequence: \\") + seq);
        err.hint = "Valid escape sequences are: \\n \\t \\r \\\\ \\\"";
        addError(err);
    }
    
private:
    std::vector<LexError> errors_;
};
