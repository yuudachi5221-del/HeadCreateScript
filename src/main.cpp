#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

static const char* usage =
"Head Create Script Compiler v0.1.0\n"
"Usage:\n"
"  hcs --help              Show this help message\n"
"  hcs --version           Show version information\n"
"  hcs <source.crs>        Compile and run source file\n"
"  hcs <source.crs> -o <out.exe>  (Future: Output to executable)\n";

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void dumpTokens(const std::vector<Token>& tokens) {
    std::cout << "=== Token Stream ===" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << i << ": Type=" << static_cast<int>(tokens[i].type)
                  << " Text=\"" << tokens[i].text << "\" "
                  << "Line=" << tokens[i].line << " Col=" << tokens[i].col << std::endl;
    }
    std::cout << "====================" << std::endl;
}

int main(int argc, char** argv) {
#ifdef HCS_VERSION
    const char* version = HCS_VERSION;
#else
    const char* version = "0.1.0";
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

    // Treat as source file
    std::string filename = arg;
    
    try {
        // Read source file
        std::string source = readFile(filename);
        
        // Lexical analysis
        Lexer lexer(source, true);
        auto tokens = lexer.tokenize();
        
        // Debug: Dump tokens if needed
        // dumpTokens(tokens);
        
        // Parsing
        Parser parser(tokens);
        auto program = parser.parse();
        
        // Interpretation
        Interpreter interpreter;
        interpreter.interpret(program);
        
        return 0;
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}
