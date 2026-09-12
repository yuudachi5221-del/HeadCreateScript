#include "semantic_analyzer.h"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() {
    globalScope_ = std::make_unique<Scope>();
    currentScope_ = globalScope_.get();
}

void SemanticAnalyzer::analyze(const std::shared_ptr<Program>& program) {
    if (!program) return;
    
    for (const auto& stmt : program->statements) {
        try {
            analyzeStatement(stmt);
        } catch (const std::runtime_error& e) {
            std::cerr << "Semantic Error: " << e.what() << std::endl;
            throw;
        }
    }
}

// ============================================================================
// Statement Analysis
// ============================================================================

void SemanticAnalyzer::analyzeStatement(const StmtPtr& stmt) {
    if (!stmt) return;
    
    if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt)) {
        analyzeExpressionStatement(exprStmt);
    } else if (auto varDecl = std::dynamic_pointer_cast<VariableDeclaration>(stmt)) {
        analyzeVariableDeclaration(varDecl);
    } else if (auto constDecl = std::dynamic_pointer_cast<ConstantDeclaration>(stmt)) {
        analyzeConstantDeclaration(constDecl);
    } else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        analyzeAssignment(assign);
    } else if (auto block = std::dynamic_pointer_cast<BlockStatement>(stmt)) {
        analyzeBlockStatement(block);
    } else if (auto ifStmt = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        analyzeIfStatement(ifStmt);
    } else if (auto forStmt = std::dynamic_pointer_cast<ForStatement>(stmt)) {
        analyzeForStatement(forStmt);
    } else if (auto dispStmt = std::dynamic_pointer_cast<DisplayStatement>(stmt)) {
        analyzeDisplayStatement(dispStmt);
    } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStatement>(stmt)) {
        analyzeWhileStatement(whileStmt);
    }
}

void SemanticAnalyzer::analyzeExpressionStatement(const std::shared_ptr<ExpressionStatement>& stmt) {
    if (stmt->expr) {
        analyzeExpression(stmt->expr);
    }
}

void SemanticAnalyzer::analyzeVariableDeclaration(const std::shared_ptr<VariableDeclaration>& stmt) {
    // Check if variable is already defined in current scope
    currentScope_->define(stmt->name, false, stmt->line, stmt->col);
    
    // Analyze initializer if present
    if (stmt->initializer) {
        analyzeExpression(stmt->initializer);
    }
}

void SemanticAnalyzer::analyzeConstantDeclaration(const std::shared_ptr<ConstantDeclaration>& stmt) {
    // Check if constant is already defined in current scope
    currentScope_->define(stmt->name, true, stmt->line, stmt->col);
    
    // Analyze initializer
    if (stmt->initializer) {
        analyzeExpression(stmt->initializer);
    }
}

void SemanticAnalyzer::analyzeAssignment(const std::shared_ptr<Assignment>& stmt) {
    // Check if variable exists
    if (!currentScope_->exists(stmt->name)) {
        throw std::runtime_error("Undefined variable '" + stmt->name + "'");
    }
    
    // Check if trying to assign to constant
    if (currentScope_->isConstant(stmt->name)) {
        throw std::runtime_error("Cannot assign to constant '" + stmt->name + "'");
    }
    
    // Analyze value expression
    if (stmt->value) {
        analyzeExpression(stmt->value);
    }
}

void SemanticAnalyzer::analyzeBlockStatement(const std::shared_ptr<BlockStatement>& stmt) {
    // Enter new scope for block
    enterScope();
    
    for (const auto& s : stmt->statements) {
        analyzeStatement(s);
    }
    
    // Exit scope
    exitScope();
}

void SemanticAnalyzer::analyzeIfStatement(const std::shared_ptr<IfStatement>& stmt) {
    // Analyze condition
    if (stmt->condition) {
        analyzeExpression(stmt->condition);
    }
    
    // Analyze then branch
    if (stmt->thenBranch) {
        analyzeStatement(stmt->thenBranch);
    }
    
    // Analyze else branch if present
    if (stmt->elseBranch) {
        analyzeStatement(stmt->elseBranch);
    }
}

void SemanticAnalyzer::analyzeForStatement(const std::shared_ptr<ForStatement>& stmt) {
    // Enter new scope for loop
    enterScope();
    
    // Analyze start expression
    if (stmt->start) {
        analyzeExpression(stmt->start);
    }
    
    // Define loop variable
    currentScope_->define(stmt->varName, false, stmt->line, stmt->col);
    
    // Analyze end expression
    if (stmt->end) {
        analyzeExpression(stmt->end);
    }
    
    // Analyze step expression
    if (stmt->step) {
        analyzeExpression(stmt->step);
    }
    
    // Analyze body
    if (stmt->body) {
        analyzeStatement(stmt->body);
    }
    
    // Exit scope
    exitScope();
}

void SemanticAnalyzer::analyzeWhileStatement(const std::shared_ptr<WhileStatement>& stmt) {
    // Analyze condition
    if (stmt->condition) {
        analyzeExpression(stmt->condition);
    }
    
    // Enter new scope for loop body
    enterScope();
    
    // Analyze body
    if (stmt->body) {
        analyzeStatement(stmt->body);
    }
    
    // Exit scope
    exitScope();
}

void SemanticAnalyzer::analyzeDisplayStatement(const std::shared_ptr<DisplayStatement>& stmt) {
    if (stmt->value) {
        analyzeExpression(stmt->value);
    }
}

// ============================================================================
// Expression Analysis
// ============================================================================

void SemanticAnalyzer::analyzeExpression(const ExprPtr& expr) {
    if (!expr) return;
    
    if (auto literal = std::dynamic_pointer_cast<LiteralExpression>(expr)) {
        analyzeLiteralExpression(literal);
    } else if (auto ident = std::dynamic_pointer_cast<IdentifierExpression>(expr)) {
        analyzeIdentifierExpression(ident);
    } else if (auto binary = std::dynamic_pointer_cast<BinaryExpression>(expr)) {
        analyzeBinaryExpression(binary);
    } else if (auto unary = std::dynamic_pointer_cast<UnaryExpression>(expr)) {
        analyzeUnaryExpression(unary);
    } else if (auto call = std::dynamic_pointer_cast<CallExpression>(expr)) {
        analyzeCallExpression(call);
    }
}

void SemanticAnalyzer::analyzeLiteralExpression(const std::shared_ptr<LiteralExpression>& expr) {
    // Literals don't need semantic analysis
}

void SemanticAnalyzer::analyzeIdentifierExpression(const std::shared_ptr<IdentifierExpression>& expr) {
    // Check if identifier is defined
    if (!currentScope_->exists(expr->name)) {
        throw std::runtime_error("Undefined variable '" + expr->name + "'");
    }
}

void SemanticAnalyzer::analyzeBinaryExpression(const std::shared_ptr<BinaryExpression>& expr) {
    // Analyze both operands
    if (expr->left) {
        analyzeExpression(expr->left);
    }
    if (expr->right) {
        analyzeExpression(expr->right);
    }
}

void SemanticAnalyzer::analyzeUnaryExpression(const std::shared_ptr<UnaryExpression>& expr) {
    // Analyze operand
    if (expr->operand) {
        analyzeExpression(expr->operand);
    }
}

void SemanticAnalyzer::analyzeCallExpression(const std::shared_ptr<CallExpression>& expr) {
    // Analyze all arguments
    for (const auto& arg : expr->arguments) {
        analyzeExpression(arg);
    }
    
    // Note: Built-in function validation could be added here
}

// ============================================================================
// Scope Management
// ============================================================================

void SemanticAnalyzer::enterScope() {
    auto newScope = std::make_unique<Scope>(currentScope_);
    currentScope_ = newScope.get();
}

void SemanticAnalyzer::exitScope() {
    if (currentScope_->getParent()) {
        currentScope_ = currentScope_->getParent();
    }
}
