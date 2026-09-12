#include <iostream>
#include <string>
#include <vector>
#include "token.h"
#include "lexer.h"

static const char* usage =
"Head Create Script Compiler (minimal)\n"
"Usage:\n"
"  hcs --help\n"
"  hcs --version\n"
"  hcs source.crs\n";

static std::string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::EOF_: return "EOF";
        case TokenType::Identifier: return "Identifier";
        case TokenType::Keyword: return "Keyword";
        case TokenType::Symbol: return "Symbol";
        case TokenType::Number: return "Number";
        case TokenType::Bool: return "Bool";
        case TokenType::Unknown: return "Unknown";
    }
    return "Unknown";
}

int main(int argc, char** argv) {
#ifdef HCS_VERSION
    const char* version = HCS_VERSION;
#else
    const char* version = "0.0.0";
#endif

    if (argc <= 1) {
        std::cout << "Head Create Script Compiler v" << version << "\n";
        std::cout << usage;
        return 0;
    }

    std::string arg = argv[1];
    if (arg == "--help") {
        std::cout << usage;
        return 0;
    }
    if (arg == "--version") {
        std::cout << "Head Create Script Compiler v" << version << "\n";
        return 0;
    }

    // treat as source file
    std::string filename = arg;
    Lexer lexer(filename);
    auto tokens = lexer.tokenize();
    for (const auto& t : tokens) {
        std::cout << tokenTypeName(t.type);
        if (!t.text.empty()) std::cout << "(" << t.text << ")";
        std::cout << "  [" << t.line << ":" << t.col << "]";
        std::cout << "\n";
    }

    return 0;
}
