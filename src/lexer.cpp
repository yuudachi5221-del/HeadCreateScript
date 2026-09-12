#include "lexer.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <unordered_set>
#include <algorithm>

static const std::unordered_set<std::string> kKeywords = {
    "con", "form", "const", "if", "ifel", "else", "for", "while", "name", "range",
    "dispin", "blank", "true", "false"
};

Lexer::Lexer(const std::string& filename) {
    initFromFile(filename);
}

Lexer::Lexer(std::string_view input, bool /*fromString*/) {
    input_.assign(input.begin(), input.end());
}

void Lexer::initFromFile(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    if (!ifs) {
        setError("Cannot open file: " + filename);
        return;
    }
    std::ostringstream ss;
    ss << ifs.rdbuf();
    input_ = ss.str();
}

char Lexer::peek(size_t offset) const {
    size_t p = pos_ + offset;
    if (p >= input_.size()) return '\0';
    return input_[p];
}

char Lexer::get() {
    if (pos_ >= input_.size()) return '\0';
    char c = input_[pos_++];
    if (c == '\n') {
        line_++;
        col_ = 1;
    } else {
        col_++;
    }
    return c;
}

bool Lexer::startsWith(const std::string& s) const {
    return input_.compare(pos_, s.size(), s) == 0;
}

bool Lexer::isIdentStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::isIdentChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool Lexer::isOctalDigit(char c) {
    return c >= '0' && c <= '7';
}

bool Lexer::isHexDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) ||
           (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Lexer::isWhitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

void Lexer::removeComments() {
    std::string out;
    size_t i = 0;
    while (i < input_.size()) {
        if (i + 1 < input_.size() && input_[i] == '<' && input_[i + 1] == '/') {
            size_t j = input_.find("/>", i + 2);
            if (j == std::string::npos) {
                // Unterminated comment: skip to end
                break;
            } else {
                i = j + 2;
                continue;
            }
        } else {
            out.push_back(input_[i]);
            i++;
        }
    }
    input_.swap(out);
}

void Lexer::skipWhitespace() {
    while (isWhitespace(peek())) {
        get();
    }
}

void Lexer::setError(const std::string& message) {
    error_state_ = true;
    error_message_ = message;
}

Token Lexer::createErrorToken(const std::string& message) {
    Token t(TokenType::Unknown, "", line_, col_);
    t.has_error = true;
    t.error_message = message;
    return t;
}

Token Lexer::recognizeKeywordOrIdentifier() {
    int l = line_;
    int c = col_;
    std::string s;
    while (isIdentChar(peek())) {
        s.push_back(get());
    }
    
    if (kKeywords.find(s) != kKeywords.end()) {
        Token tk(TokenType::Keyword, s, l, c);
        // Special handling for boolean literals
        if (s == "true") {
            tk.type = TokenType::Bool;
            tk.value = "true";
        } else if (s == "false") {
            tk.type = TokenType::Bool;
            tk.value = "false";
        }
        return tk;
    } else {
        return Token(TokenType::Identifier, s, l, c);
    }
}

Token Lexer::recognizeNumber() {
    int l = line_;
    int c = col_;
    std::string s;
    
    // Handle hexadecimal (0x...)
    if (peek() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
        s.push_back(get()); // '0'
        s.push_back(get()); // 'x' or 'X'
        
        if (!isHexDigit(peek())) {
            return createErrorToken("Invalid hexadecimal literal at " +
                                    std::to_string(l) + ":" + std::to_string(c));
        }
        
        while (isHexDigit(peek())) {
            s.push_back(get());
        }
        return Token(TokenType::Number, s, l, c, s);
    }
    
    // Handle octal (0...)
    if (peek() == '0' && isOctalDigit(peek(1))) {
        s.push_back(get()); // '0'
        while (isOctalDigit(peek())) {
            s.push_back(get());
        }
        return Token(TokenType::Number, s, l, c, s);
    }
    
    // Handle decimal or float
    while (isDigit(peek())) {
        s.push_back(get());
    }
    
    // Check for decimal point
    if (peek() == '.' && isDigit(peek(1))) {
        s.push_back(get()); // '.'
        while (isDigit(peek())) {
            s.push_back(get());
        }
    }
    
    // Check for scientific notation (e.g., 1.5e-3)
    if ((peek() == 'e' || peek() == 'E')) {
        char next = peek(1);
        if (isDigit(next) || ((next == '+' || next == '-') && isDigit(peek(2)))) {
            s.push_back(get()); // 'e' or 'E'
            if (peek() == '+' || peek() == '-') {
                s.push_back(get());
            }
            while (isDigit(peek())) {
                s.push_back(get());
            }
        }
    }
    
    return Token(TokenType::Number, s, l, c, s);
}

Token Lexer::recognizeString() {
    int l = line_;
    int c = col_;
    std::string s;
    
    get(); // consume opening quote
    
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') {
            get(); // consume backslash
            char escaped = peek();
            if (escaped == '\0') {
                return createErrorToken("Unterminated string at " +
                                        std::to_string(l) + ":" + std::to_string(c));
            }
            get();
            // Handle escape sequences
            switch (escaped) {
                case 'n': s.push_back('\n'); break;
                case 't': s.push_back('\t'); break;
                case 'r': s.push_back('\r'); break;
                case '\\': s.push_back('\\'); break;
                case '"': s.push_back('"'); break;
                default: s.push_back(escaped); break;
            }
        } else {
            s.push_back(get());
        }
    }
    
    if (peek() != '"') {
        return createErrorToken("Unterminated string at " +
                                std::to_string(l) + ":" + std::to_string(c));
    }
    
    get(); // consume closing quote
    return Token(TokenType::String, "\"" + s + "\"", l, c, s);
}

Token Lexer::recognizeOperatorOrSymbol() {
    int l = line_;
    int c = col_;
    
    // Two-character operators
    if (startsWith("<-")) {
        get();
        get();
        return Token(TokenType::Arrow, "<-", l, c);
    }
    if (startsWith("==")) {
        get();
        get();
        return Token(TokenType::Equal, "==", l, c);
    }
    if (startsWith("!=")) {
        get();
        get();
        return Token(TokenType::NotEqual, "!=", l, c);
    }
    if (startsWith("<=")) {
        get();
        get();
        return Token(TokenType::LessEq, "<=", l, c);
    }
    if (startsWith(">=")) {
        get();
        get();
        return Token(TokenType::GreaterEq, ">=", l, c);
    }
    
    // Single-character tokens
    char ch = get();
    switch (ch) {
        case '+': return Token(TokenType::Plus, "+", l, c);
        case '-': return Token(TokenType::Minus, "-", l, c);
        case '*': return Token(TokenType::Star, "*", l, c);
        case '/': return Token(TokenType::Slash, "/", l, c);
        case '%': return Token(TokenType::Percent, "%", l, c);
        case '=': return Token(TokenType::Assign, "=", l, c);
        case '<': return Token(TokenType::Less, "<", l, c);
        case '>': return Token(TokenType::Greater, ">", l, c);
        case '(': return Token(TokenType::LParen, "(", l, c);
        case ')': return Token(TokenType::RParen, ")", l, c);
        case '{': return Token(TokenType::LBrace, "{", l, c);
        case '}': return Token(TokenType::RBrace, "}", l, c);
        case ';': return Token(TokenType::Semicolon, ";", l, c);
        default:
            return Token(TokenType::Unknown, std::string(1, ch), l, c);
    }
}

std::vector<Token> Lexer::tokenize() {
    removeComments();
    pos_ = 0;
    line_ = 1;
    col_ = 1;
    error_state_ = false;
    error_message_.clear();
    
    std::vector<Token> tokens;
    
    while (true) {
        skipWhitespace();
        char c = peek();
        
        if (c == '\0') {
            tokens.emplace_back(TokenType::EOF_, "", line_, col_);
            break;
        }
        
        // Identifiers and keywords
        if (isIdentStart(c)) {
            tokens.push_back(recognizeKeywordOrIdentifier());
            continue;
        }
        
        // Numbers
        if (isDigit(c)) {
            tokens.push_back(recognizeNumber());
            continue;
        }
        
        // Strings
        if (c == '"') {
            tokens.push_back(recognizeString());
            continue;
        }
        
        // Operators and symbols
        Token opToken = recognizeOperatorOrSymbol();
        if (opToken.type == TokenType::Unknown) {
            setError("Unknown character '" + std::string(1, c) + "' at " +
                     std::to_string(line_) + ":" + std::to_string(col_));
        }
        tokens.push_back(opToken);
    }
    
    return tokens;
}
