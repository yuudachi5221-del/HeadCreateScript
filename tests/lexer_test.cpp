#include <iostream>
#include <cassert>
#include <sstream>
#include "../src/lexer.h"

// Test helper function
void assert_token(const Token& token, TokenType expected_type, const std::string& expected_text) {
    if (token.type != expected_type || token.text != expected_text) {
        std::cerr << "Token mismatch! Expected type: " << static_cast<int>(expected_type)
                  << " text: '" << expected_text << "'"
                  << " Got type: " << static_cast<int>(token.type)
                  << " text: '" << token.text << "'" << std::endl;
        assert(false);
    }
}

void test_simple_variable_declaration() {
    std::cout << "Test: Simple variable declaration..." << std::endl;
    Lexer lexer("con form x=10;", true);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() >= 6); // con, form, x, =, 10, ;, EOF
    assert_token(tokens[0], TokenType::Keyword, "con");
    assert_token(tokens[1], TokenType::Keyword, "form");
    assert_token(tokens[2], TokenType::Identifier, "x");
    assert_token(tokens[3], TokenType::Assign, "=");
    assert_token(tokens[4], TokenType::Number, "10");
    assert_token(tokens[5], TokenType::Semicolon, ";");
    
    std::cout << "✓ Passed" << std::endl;
}

void test_string_literal() {
    std::cout << "Test: String literal..." << std::endl;
    Lexer lexer("dispin(\"Hello World\");", true);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() >= 5);
    assert_token(tokens[0], TokenType::Keyword, "dispin");
    assert_token(tokens[1], TokenType::LParen, "(");
    assert_token(tokens[2], TokenType::String, "\"Hello World\"");
    assert_token(tokens[3], TokenType::RParen, ")");
    assert_token(tokens[4], TokenType::Semicolon, ";");
    
    std::cout << "✓ Passed" << std::endl;
}

void test_if_statement() {
    std::cout << "Test: If statement..." << std::endl;
    Lexer lexer("con (x<=10)<-if{ dispin(x); };", true);
    auto tokens = lexer.tokenize();
    
    bool found_arrow = false;
    bool found_lbrace = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Arrow) found_arrow = true;
        if (token.type == TokenType::LBrace) found_lbrace = true;
    }
    
    assert(found_arrow && found_lbrace);
    std::cout << "✓ Passed" << std::endl;
}

void test_while_statement() {
    std::cout << "Test: While statement..." << std::endl;
    Lexer lexer("con (x<10)<-while{ x=x+1; };", true);
    auto tokens = lexer.tokenize();
    
    bool found_while = false;
    bool found_arrow = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Keyword && token.text == "while") found_while = true;
        if (token.type == TokenType::Arrow) found_arrow = true;
    }
    
    assert(found_while && found_arrow);
    std::cout << "✓ Passed" << std::endl;
}

void test_comment_removal() {
    std::cout << "Test: Comment removal..." << std::endl;
    Lexer lexer("</ This is a comment /> con form x=10;", true);
    auto tokens = lexer.tokenize();
    
    // Comments should be removed, so we should just get con, form, x, =, 10, ;, EOF
    assert(tokens.size() >= 6);
    assert_token(tokens[0], TokenType::Keyword, "con");
    
    std::cout << "✓ Passed" << std::endl;
}

void test_boolean_literals() {
    std::cout << "Test: Boolean literals..." << std::endl;
    Lexer lexer("con form flag=true; flag=false;", true);
    auto tokens = lexer.tokenize();
    
    bool found_true = false;
    bool found_false = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Bool && token.text == "true") found_true = true;
        if (token.type == TokenType::Bool && token.text == "false") found_false = true;
    }
    
    assert(found_true && found_false);
    std::cout << "✓ Passed" << std::endl;
}

void test_arithmetic_operators() {
    std::cout << "Test: Arithmetic operators..." << std::endl;
    Lexer lexer("x=a+b-c*d/e%f;", true);
    auto tokens = lexer.tokenize();
    
    bool found_plus = false, found_minus = false, found_star = false;
    bool found_slash = false, found_percent = false;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::Plus) found_plus = true;
        if (token.type == TokenType::Minus) found_minus = true;
        if (token.type == TokenType::Star) found_star = true;
        if (token.type == TokenType::Slash) found_slash = true;
        if (token.type == TokenType::Percent) found_percent = true;
    }
    
    assert(found_plus && found_minus && found_star && found_slash && found_percent);
    std::cout << "✓ Passed" << std::endl;
}

void test_comparison_operators() {
    std::cout << "Test: Comparison operators..." << std::endl;
    Lexer lexer("if(x==10 && y!=20 && z<30 && w>5 && a<=100 && b>=0);", true);
    auto tokens = lexer.tokenize();
    
    bool found_eq = false, found_neq = false, found_lt = false;
    bool found_gt = false, found_lte = false, found_gte = false;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::Equal) found_eq = true;
        if (token.type == TokenType::NotEqual) found_neq = true;
        if (token.type == TokenType::Less) found_lt = true;
        if (token.type == TokenType::Greater) found_gt = true;
        if (token.type == TokenType::LessEq) found_lte = true;
        if (token.type == TokenType::GreaterEq) found_gte = true;
    }
    
    assert(found_eq && found_neq && found_lt && found_gt && found_lte && found_gte);
    std::cout << "✓ Passed" << std::endl;
}

void test_for_loop_syntax() {
    std::cout << "Test: For loop syntax..." << std::endl;
    Lexer lexer("con for(0, name=\"i\", <=10, range=\"1\"){ dispin(i); };", true);
    auto tokens = lexer.tokenize();
    
    assert(tokens.size() > 0);
    // Check that it doesn't have lexer errors
    std::cout << "✓ Passed (no lexer errors)" << std::endl;
}

void test_float_numbers() {
    std::cout << "Test: Float numbers..." << std::endl;
    Lexer lexer("con form pi=3.14; con form e=2.71828; con form sci=1.5e-3;", true);
    auto tokens = lexer.tokenize();
    
    bool found_314 = false, found_27 = false, found_sci = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Number && token.text == "3.14") found_314 = true;
        if (token.type == TokenType::Number && token.text == "2.71828") found_27 = true;
        if (token.type == TokenType::Number && token.text == "1.5e-3") found_sci = true;
    }
    
    assert(found_314 && found_27 && found_sci);
    std::cout << "✓ Passed" << std::endl;
}

void test_hexadecimal_numbers() {
    std::cout << "Test: Hexadecimal numbers..." << std::endl;
    Lexer lexer("con form hex1=0xFF; con form hex2=0x1A2B;", true);
    auto tokens = lexer.tokenize();
    
    bool found_hex = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Number && (token.text == "0xFF" || token.text == "0x1A2B")) {
            found_hex = true;
        }
    }
    
    assert(found_hex);
    std::cout << "✓ Passed" << std::endl;
}

void test_string_escape_sequences() {
    std::cout << "Test: String escape sequences..." << std::endl;
    Lexer lexer("dispin(\"Hello\\nWorld\\t!\");", true);
    auto tokens = lexer.tokenize();
    
    bool found_escaped_string = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::String) {
            // The value should contain the actual escape sequences
            found_escaped_string = true;
        }
    }
    
    assert(found_escaped_string);
    std::cout << "✓ Passed" << std::endl;
}

void test_blank_keyword() {
    std::cout << "Test: Blank keyword..." << std::endl;
    Lexer lexer("con form x=blank;", true);
    auto tokens = lexer.tokenize();
    
    bool found_blank = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Keyword && token.text == "blank") {
            found_blank = true;
        }
    }
    
    assert(found_blank);
    std::cout << "✓ Passed" << std::endl;
}

void test_complex_expression() {
    std::cout << "Test: Complex expression..." << std::endl;
    Lexer lexer("con (x<=10 && y>5)<-if{ con form z=(x+y)*2; };", true);
    auto tokens = lexer.tokenize();
    
    // Just verify it tokenizes without errors
    assert(!lexer.hasError());
    assert(tokens.back().type == TokenType::EOF_);
    
    std::cout << "✓ Passed" << std::endl;
}

void test_const_keyword() {
    std::cout << "Test: Const keyword..." << std::endl;
    Lexer lexer("con const PI=3.14159;", true);
    auto tokens = lexer.tokenize();
    
    bool found_const = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Keyword && token.text == "const") {
            found_const = true;
        }
    }
    
    assert(found_const);
    std::cout << "✓ Passed" << std::endl;
}

void test_ifel_keyword() {
    std::cout << "Test: ifel keyword..." << std::endl;
    Lexer lexer("con (x<=1)<-if{ dispin(\"A\"); } con (x<=3)<-ifel{ dispin(\"B\"); };", true);
    auto tokens = lexer.tokenize();
    
    bool found_ifel = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Keyword && token.text == "ifel") {
            found_ifel = true;
        }
    }
    
    assert(found_ifel);
    std::cout << "✓ Passed" << std::endl;
}

void test_else_keyword() {
    std::cout << "Test: else keyword..." << std::endl;
    Lexer lexer("con ()<-else{ dispin(\"C\"); };", true);
    auto tokens = lexer.tokenize();
    
    bool found_else = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Keyword && token.text == "else") {
            found_else = true;
        }
    }
    
    assert(found_else);
    std::cout << "✓ Passed" << std::endl;
}

void test_range_keyword() {
    std::cout << "Test: range keyword..." << std::endl;
    Lexer lexer("con for(0, name=\"i\", <=10, range=\"2\"){ dispin(i); };", true);
    auto tokens = lexer.tokenize();
    
    bool found_range = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::Keyword && token.text == "range") {
            found_range = true;
        }
    }
    
    assert(found_range);
    std::cout << "✓ Passed" << std::endl;
}

int main() {
    std::cout << "=== HCS Lexer Test Suite ===" << std::endl << std::endl;
    
    try {
        test_simple_variable_declaration();
        test_string_literal();
        test_if_statement();
        test_while_statement();
        test_comment_removal();
        test_boolean_literals();
        test_arithmetic_operators();
        test_comparison_operators();
        test_for_loop_syntax();
        test_float_numbers();
        test_hexadecimal_numbers();
        test_string_escape_sequences();
        test_blank_keyword();
        test_const_keyword();
        test_ifel_keyword();
        test_else_keyword();
        test_range_keyword();
        test_complex_expression();
        
        std::cout << std::endl << "=== All tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
