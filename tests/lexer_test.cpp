#include "lexer.h"
#include "token.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>

static int run_tests() {
    int failures = 0;
    {
        // Test 1: simple keywords and symbols
        const char* text = "con name = foo; if true { dispin }";
        Lexer lx(std::string_view(text), true);
        auto toks = lx.tokenize();
        // Expect: con(kw), name(kw), =(sym), foo(id), ;(sym), if(kw), true(bool), {(sym), dispin(kw), }(sym), EOF
        size_t idx = 0;
        auto expect = [&](TokenType type, const std::string& txt){
            if (idx >= toks.size()) { std::cerr << "Unexpected end of tokens\n"; failures++; return; }
            if (toks[idx].type != type || (txt.size() && toks[idx].text != txt)) {
                std::cerr << "Test1 token " << idx << " mismatch: got (" << int(toks[idx].type) << ",\"" << toks[idx].text << "\") expected (" << int(type) << ",\"" << txt << "\")\n";
                failures++;
            }
            idx++;
        };
        expect(TokenType::Keyword, "con");
        expect(TokenType::Keyword, "name");
        expect(TokenType::Symbol, "=");
        expect(TokenType::Identifier, "foo");
        expect(TokenType::Symbol, ";");
        expect(TokenType::Keyword, "if");
        expect(TokenType::Bool, "true");
        expect(TokenType::Symbol, "{");
        expect(TokenType::Keyword, "dispin");
        expect(TokenType::Symbol, "}");
    }

    {
        // Test 2: comment removal
        const char* text = "con a <- 1; </ this is a comment /> dispin;";
        Lexer lx(std::string_view(text), true);
        auto toks = lx.tokenize();
        // Expect: con, a, <-, 1, ;, dispin, ;, EOF
        std::vector<std::pair<TokenType,std::string>> expectList = {
            {TokenType::Keyword, "con"},
            {TokenType::Identifier, "a"},
            {TokenType::Symbol, "<-"},
            {TokenType::Number, "1"},
            {TokenType::Symbol, ";"},
            {TokenType::Keyword, "dispin"},
            {TokenType::Symbol, ";"}
        };
        size_t i=0;
        for (auto &e : expectList) {
            if (i >= toks.size()) { std::cerr << "Unexpected end in Test2\n"; failures++; break; }
            if (toks[i].type != e.first || toks[i].text != e.second) {
                std::cerr << "Test2 token "<<i<<" mismatch: got ("<<int(toks[i].type)<<",\""<<toks[i].text<<"\") expected ("<<int(e.first)<<",\""<<e.second<<"\")\n";
                failures++;
            }
            i++;
        }
    }

    {
        // Test 3: file reading
        const char* fname = "lexer_test_input.crs";
        std::ofstream ofs(fname);
        ofs << "form range {\n  name = x; \n  blank </comment/>;\n}";
        ofs.close();
        Lexer lx(std::string(fname));
        auto toks = lx.tokenize();
        // Expect form(kw), range(kw), {(sym), name(kw), =(sym), x(id), ;(sym), blank(kw), ;(sym), }(sym)
        std::vector<std::pair<TokenType,std::string>> expectList = {
            {TokenType::Keyword, "form"},
            {TokenType::Keyword, "range"},
            {TokenType::Symbol, "{"},
            {TokenType::Keyword, "name"},
            {TokenType::Symbol, "="},
            {TokenType::Identifier, "x"},
            {TokenType::Symbol, ";"},
            {TokenType::Keyword, "blank"},
            {TokenType::Symbol, ";"},
            {TokenType::Symbol, "}"}
        };
        size_t i=0;
        for (auto &e : expectList) {
            if (i >= toks.size()) { std::cerr << "Unexpected end in Test3\n"; failures++; break; }
            if (toks[i].type != e.first || toks[i].text != e.second) {
                std::cerr << "Test3 token "<<i<<" mismatch: got ("<<int(toks[i].type)<<",\""<<toks[i].text<<"\") expected ("<<int(e.first)<<",\""<<e.second<<"\")\n";
                failures++;
            }
            i++;
        }
        // remove temp file
        std::remove(fname);
    }

    return failures;
}

int main() {
    int failures = run_tests();
    if (failures == 0) {
        std::cout << "All lexer tests passed\n";
        return 0;
    } else {
        std::cerr << failures << " tests failed\n";
        return 1;
    }
}
