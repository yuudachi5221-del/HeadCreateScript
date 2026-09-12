#pragma once

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include "token.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    
    std::shared_ptr<Program> parse();
    
private:
    std::vector<Token> tokens_;
    size_t current_ = 0;
    
    // Helper functions
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(const std::string& type) const;
    bool match(const std::string& type);
    bool match(const std::vector<std::string>& types);
    const Token& consume(const std::string& type, const std::string& message);
    bool isAtEnd() const;
    
    // Parsing functions
    StmtPtr statement();
    StmtPtr declaration();
    StmtPtr variableDeclaration();
    StmtPtr constantDeclaration();
    StmtPtr ifStatement();
    StmtPtr forStatement();
    StmtPtr displayStatement();
    StmtPtr expressionStatement();
    StmtPtr blockStatement();
    
    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr logicalOr();
    ExprPtr logicalAnd();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr call();
    ExprPtr primary();
    
    // Error handling
    void error(const std::string& message);
    void synchronize();
};
