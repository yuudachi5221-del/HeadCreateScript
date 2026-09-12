#include "lexer.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <unordered_set>

static const std::unordered_set<std::string> kKeywords = {
    "con","form","const","if","ifel","else","for","name","range",
    "dispin","blank","true","false"
};

Lexer::Lexer(const std::string& filename) {
    initFromFile(filename);
}

Lexer::Lexer(std::string_view input, bool /*fromString*/) {
    input_.assign(input.begin(), input.end());
}

void Lexer::initFromFile(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::in | std::ios::binary);
    if (!ifs) return;
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
    if (c == '\n') { line_++; col_ = 1; } else { col_++; }
    return c;
}

bool Lexer::startsWith(const std::string& s) const {
    return input_.compare(pos_, s.size(), s) == 0;
}

void Lexer::removeComments() {
    // Remove comments of the form </ ... /> (non-greedy)
    std::string out;
    size_t i = 0;
    while (i < input_.size()) {
        if (i + 1 < input_.size() && input_[i] == '<' && input_[i+1] == '/') {
            // find "/>" after i+2
            size_t j = input_.find("/>", i+2);
            if (j == std::string::npos) {
                // skip to end
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
    while (std::isspace(static_cast<unsigned char>(peek()))) get();
}

bool Lexer::isIdentStart(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool Lexer::isIdentChar(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

std::vector<Token> Lexer::tokenize() {
    removeComments();
    pos_ = 0; line_ = 1; col_ = 1;
    std::vector<Token> tokens;
    while (true) {
        skipWhitespace();
        char c = peek();
        if (c == '\0') {
            tokens.emplace_back(TokenType::EOF_, "", line_, col_);
            break;
        }
        if (isIdentStart(c)) {
            int l = line_, col = col_;
            std::string s;
            while (isIdentChar(peek())) s.push_back(get());
            if (kKeywords.find(s) != kKeywords.end()) {
                Token tk(TokenType::Keyword, s, l, col);
                if (s == "true" || s == "false") tk.type = TokenType::Bool;
                tokens.push_back(std::move(tk));
            } else {
                tokens.emplace_back(TokenType::Identifier, s, l, col);
            }
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c))) {
            int l = line_, col = col_;
            std::string s;
            while (std::isdigit(static_cast<unsigned char>(peek()))) s.push_back(get());
            tokens.emplace_back(TokenType::Number, s, l, col);
            continue;
        }
        // symbols: <-, ;, =, (, ), {, }
        if (startsWith("<-")) {
            int l = line_, col = col_;
            get(); get();
            tokens.emplace_back(TokenType::Symbol, "<-", l, col);
            continue;
        }
        // single-char symbols
        char ch = get();
        std::string sch(1, ch);
        if (ch == ';' || ch == '=' || ch == '(' || ch == ')' || ch == '{' || ch == '}') {
            tokens.emplace_back(TokenType::Symbol, sch, line_, col_ - 1);
            continue;
        }
        // unknown, produce Unknown token
        tokens.emplace_back(TokenType::Unknown, sch, line_, col_ - 1);
    }
    return tokens;
}
