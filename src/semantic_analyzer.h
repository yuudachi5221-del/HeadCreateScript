#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include "ast.h"

struct SymbolInfo {
    std::string name;
    bool isConstant = false;
    bool isDefined = false;
    int line = 0;
    int col = 0;
};

class Scope {
public:
    explicit Scope(Scope* parent = nullptr) : parent_(parent) {}
    
    void define(const std::string& name, bool isConstant = false, int line = 0, int col = 0) {
        if (symbols_.find(name) != symbols_.end() && symbols_[name].isDefined) {
            throw std::runtime_error("Variable '" + name + "' already defined in this scope (line " + 
                                   std::to_string(line) + ":" + std::to_string(col) + ")");
        }
        symbols_[name] = SymbolInfo{name, isConstant, true, line, col};
    }
    
    bool exists(const std::string& name) const {
        if (symbols_.find(name) != symbols_.end()) {
            return symbols_.at(name).isDefined;
        }
        if (parent_) {
            return parent_->exists(name);
        }
        return false;
    }
    
    bool isConstant(const std::string& name) const {
        auto it = symbols_.find(name);
        if (it != symbols_.end() && it->second.isDefined) {
            return it->second.isConstant;
        }
        if (parent_) {
            return parent_->isConstant(name);
        }
        return false;
    }
    
    Scope* getParent() const { return parent_; }
    
private:
    std::unordered_map<std::string, SymbolInfo> symbols_;
    Scope* parent_;
};

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    
    void analyze(const std::shared_ptr<Program>& program);
    
private:
    std::unique_ptr<Scope> globalScope_;
    Scope* currentScope_ = nullptr;
    
    void analyzeStatement(const StmtPtr& stmt);
    void analyzeExpression(const ExprPtr& expr);
    
    void analyzeExpressionStatement(const std::shared_ptr<ExpressionStatement>& stmt);
    void analyzeVariableDeclaration(const std::shared_ptr<VariableDeclaration>& stmt);
    void analyzeConstantDeclaration(const std::shared_ptr<ConstantDeclaration>& stmt);
    void analyzeAssignment(const std::shared_ptr<Assignment>& stmt);
    void analyzeBlockStatement(const std::shared_ptr<BlockStatement>& stmt);
    void analyzeIfStatement(const std::shared_ptr<IfStatement>& stmt);
    void analyzeForStatement(const std::shared_ptr<ForStatement>& stmt);
    void analyzeDisplayStatement(const std::shared_ptr<DisplayStatement>& stmt);
    
    void analyzeLiteralExpression(const std::shared_ptr<LiteralExpression>& expr);
    void analyzeIdentifierExpression(const std::shared_ptr<IdentifierExpression>& expr);
    void analyzeBinaryExpression(const std::shared_ptr<BinaryExpression>& expr);
    void analyzeUnaryExpression(const std::shared_ptr<UnaryExpression>& expr);
    void analyzeCallExpression(const std::shared_ptr<CallExpression>& expr);
    
    void enterScope();
    void exitScope();
};
