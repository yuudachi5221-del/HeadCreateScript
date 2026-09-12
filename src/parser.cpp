#include "parser.h"
#include <iostream>
#include <algorithm>

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::shared_ptr<Program> Parser::parse() {
    auto program = std::make_shared<Program>();
    while (!isAtEnd()) {
        try {
            auto stmt = statement();
            if (stmt) program->statements.push_back(stmt);
        } catch (const std::runtime_error&) {
            synchronize();
        }
    }
    return program;
}

// ============================================================================
// Helper Functions
// ============================================================================

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::check(const std::string& type) const {
    if (isAtEnd()) return false;
    return peek().text == type;
}

bool Parser::match(const std::string& type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::vector<std::string>& types) {
    for (const auto& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

const Token& Parser::consume(const std::string& type, const std::string& message) {
    if (check(type)) return advance();
    error(message);
    throw std::runtime_error(message);
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_;
}

void Parser::error(const std::string& message) {
    std::cerr << "Line " << peek().line << ":" << peek().col 
              << " Error: " << message << std::endl;
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().text == ";") return;
        if (check("con") || check("if") || check("for")) return;
        advance();
    }
}

// ============================================================================
// Statement Parsing
// ============================================================================

StmtPtr Parser::statement() {
    if (check("con")) return declaration();
    if (check("dispin")) return displayStatement();
    if (peek().type == TokenType::Identifier) {
        // Could be assignment
        size_t savedPos = current_;
        std::string name = advance().text;
        if (match("=")) {
            current_ = savedPos;
            return expressionStatement();
        }
        current_ = savedPos;
    }
    return expressionStatement();
}

StmtPtr Parser::declaration() {
    consume("con", "Expected 'con'");
    
    if (check("form")) {
        return variableDeclaration();
    }
    if (check("const")) {
        return constantDeclaration();
    }
    if (check("(")) {
        return ifStatement();
    }
    if (check("for")) {
        return forStatement();
    }
    
    error("Expected 'form', 'const', 'for', or condition after 'con'");
    throw std::runtime_error("Invalid declaration");
}

StmtPtr Parser::variableDeclaration() {
    consume("form", "Expected 'form'");
    std::string name = consume("Identifier", "Expected variable name").text;
    
    ExprPtr init = nullptr;
    if (match("=")) {
        init = expression();
    }
    
    consume(";", "Expected ';' after variable declaration");
    return std::make_shared<VariableDeclaration>(name, init);
}

StmtPtr Parser::constantDeclaration() {
    consume("const", "Expected 'const'");
    std::string name = consume("Identifier", "Expected constant name").text;
    consume("=", "Expected '=' in constant declaration");
    auto init = expression();
    consume(";", "Expected ';' after constant declaration");
    return std::make_shared<ConstantDeclaration>(name, init);
}

StmtPtr Parser::ifStatement() {
    // con (<condition>)<-if { ... };
    consume("(", "Expected '('");
    auto condition = expression();
    consume(")", "Expected ')'");
    consume("<-", "Expected '<-'");
    
    std::string keyword = consume("Keyword", "Expected 'if', 'ifel', or 'else'").text;
    
    consume("{", "Expected '{'");
    auto thenBranch = blockStatement();
    
    // Check for else/ifel
    StmtPtr elseBranch = nullptr;
    if (check("con") && current_ + 1 < tokens_.size() && 
        tokens_[current_ + 1].text == "(") {
        // This could be an ifel or else
        size_t savedPos = current_;
        advance(); // consume 'con'
        advance(); // consume '('
        
        // Peek ahead to see if it's ifel or else
        if (tokens_[savedPos + 2].text == ")") {
            // It's an else - rewind and parse
            current_ = savedPos;
            elseBranch = std::make_shared<ExpressionStatement>(nullptr);
            // Will be handled by next con statement
        } else {
            // It's an ifel - rewind and let recursion handle it
            current_ = savedPos;
            elseBranch = declaration();
        }
    }
    
    consume(";", "Expected ';' after if statement");
    return std::make_shared<IfStatement>(condition, thenBranch, elseBranch);
}

StmtPtr Parser::forStatement() {
    // con for( <start>, name="<var>", <op><end>, range="<step>" ) { ... };
    consume("for", "Expected 'for'");
    consume("(", "Expected '('");
    
    auto start = expression();
    consume(",", "Expected ','");
    
    consume("name", "Expected 'name'");
    consume("=", "Expected '='");
    std::string varName = consume("String", "Expected string for variable name").text;
    // Remove quotes
    if (varName.front() == '"') varName = varName.substr(1, varName.length() - 2);
    
    consume(",", "Expected ','");
    
    // Parse comparison operator and end expression
    std::string endOp = peek().text;
    if (!match({"<=", ">=", "<", ">", "==", "!="})) {
        error("Expected comparison operator");
        throw std::runtime_error("Invalid comparison operator");
    }
    auto end = expression();
    
    ExprPtr step = nullptr;
    if (match(",")) {
        consume("range", "Expected 'range'");
        consume("=", "Expected '='");
        step = expression();
    } else {
        // Default step is 1
        step = std::make_shared<LiteralExpression>(LiteralExpression::Type::Integer, "1");
    }
    
    consume(")", "Expected ')'");
    consume("{", "Expected '{'");
    auto body = blockStatement();
    consume("}", "Expected '}'");
    consume(";", "Expected ';' after for statement");
    
    return std::make_shared<ForStatement>(start, varName, end, endOp, step, body);
}

StmtPtr Parser::displayStatement() {
    consume("dispin", "Expected 'dispin'");
    consume("(", "Expected '('");
    auto value = expression();
    consume(")", "Expected ')'");
    consume(";", "Expected ';' after dispin");
    return std::make_shared<DisplayStatement>(value);
}

StmtPtr Parser::expressionStatement() {
    auto expr = expression();
    consume(";", "Expected ';' after expression");
    return std::make_shared<ExpressionStatement>(expr);
}

StmtPtr Parser::blockStatement() {
    auto block = std::make_shared<BlockStatement>();
    while (!check("}") && !isAtEnd()) {
        auto stmt = statement();
        if (stmt) block->statements.push_back(stmt);
    }
    consume("}", "Expected '}'");
    return block;
}

// ============================================================================
// Expression Parsing
// ============================================================================

ExprPtr Parser::expression() {
    return assignment();
}

ExprPtr Parser::assignment() {
    auto expr = logicalOr();
    
    if (match("=")) {
        auto value = assignment();
        if (auto* ident = dynamic_cast<IdentifierExpression*>(expr.get())) {
            return std::make_shared<BinaryExpression>(BinaryExpression::Op::Eq,
                std::make_shared<IdentifierExpression>(ident->name),
                value);
        }
        error("Invalid assignment target");
    }
    return expr;
}

ExprPtr Parser::logicalOr() {
    return logicalAnd();  // Simplified for now
}

ExprPtr Parser::logicalAnd() {
    return equality();  // Simplified for now
}

ExprPtr Parser::equality() {
    auto expr = comparison();
    
    while (match({"==", "!="})) {
        std::string op = previous().text;
        auto right = comparison();
        auto binOp = (op == "==") ? BinaryExpression::Op::Eq : BinaryExpression::Op::Ne;
        expr = std::make_shared<BinaryExpression>(binOp, expr, right);
    }
    return expr;
}

ExprPtr Parser::comparison() {
    auto expr = term();
    
    while (match({"<", ">", "<=", ">="})) {
        std::string op = previous().text;
        auto right = term();
        BinaryExpression::Op binOp;
        if (op == "<") binOp = BinaryExpression::Op::Lt;
        else if (op == ">") binOp = BinaryExpression::Op::Gt;
        else if (op == "<=") binOp = BinaryExpression::Op::Lte;
        else binOp = BinaryExpression::Op::Gte;
        expr = std::make_shared<BinaryExpression>(binOp, expr, right);
    }
    return expr;
}

ExprPtr Parser::term() {
    auto expr = factor();
    
    while (match({"+", "-"})) {
        std::string op = previous().text;
        auto right = factor();
        auto binOp = (op == "+") ? BinaryExpression::Op::Add : BinaryExpression::Op::Sub;
        expr = std::make_shared<BinaryExpression>(binOp, expr, right);
    }
    return expr;
}

ExprPtr Parser::factor() {
    auto expr = unary();
    
    while (match({"*", "/", "%"})) {
        std::string op = previous().text;
        auto right = unary();
        BinaryExpression::Op binOp;
        if (op == "*") binOp = BinaryExpression::Op::Mul;
        else if (op == "/") binOp = BinaryExpression::Op::Div;
        else binOp = BinaryExpression::Op::Mod;
        expr = std::make_shared<BinaryExpression>(binOp, expr, right);
    }
    return expr;
}

ExprPtr Parser::unary() {
    if (match({"-"})) {
        auto expr = unary();
        return std::make_shared<UnaryExpression>(UnaryExpression::Op::Neg, expr);
    }
    return call();
}

ExprPtr Parser::call() {
    auto expr = primary();
    
    while (true) {
        if (match("(")) {
            // Function call
            if (auto* ident = dynamic_cast<IdentifierExpression*>(expr.get())) {
                auto call = std::make_shared<CallExpression>(ident->name);
                
                if (!check(")")) {
                    do {
                        call->arguments.push_back(expression());
                    } while (match(","));
                }
                
                consume(")", "Expected ')' after arguments");
                expr = call;
            }
        } else {
            break;
        }
    }
    return expr;
}

ExprPtr Parser::primary() {
    if (match("true")) {
        return std::make_shared<LiteralExpression>(
            LiteralExpression::Type::Bool, "true");
    }
    if (match("false")) {
        return std::make_shared<LiteralExpression>(
            LiteralExpression::Type::Bool, "false");
    }
    if (match("blank")) {
        return std::make_shared<LiteralExpression>(
            LiteralExpression::Type::Blank, "blank");
    }
    
    if (peek().type == TokenType::Number) {
        auto token = advance();
        // Check if it's a float
        if (token.text.find('.') != std::string::npos) {
            return std::make_shared<LiteralExpression>(
                LiteralExpression::Type::Double, token.text);
        } else {
            return std::make_shared<LiteralExpression>(
                LiteralExpression::Type::Integer, token.text);
        }
    }
    
    if (peek().type == TokenType::Identifier) {
        auto name = advance().text;
        return std::make_shared<IdentifierExpression>(name);
    }
    
    if (match("(")) {
        auto expr = expression();
        consume(")", "Expected ')' after expression");
        return expr;
    }
    
    error("Expected expression");
    throw std::runtime_error("Expected expression");
}
