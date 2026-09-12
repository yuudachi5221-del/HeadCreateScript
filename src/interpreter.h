#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include "ast.h"
#include "hcs_value.h"

class Interpreter {
public:
    Interpreter();
    
    void interpret(const std::shared_ptr<Program>& program);
    
private:
    std::unordered_map<std::string, HCSValue> globalVariables_;
    std::unordered_map<std::string, bool> constantFlags_;  // Track which vars are const
    
    HCSValue executeStatement(const StmtPtr& stmt);
    HCSValue executeExpression(const ExprPtr& expr);
    
    // Statement execution
    HCSValue executeExpressionStatement(const std::shared_ptr<ExpressionStatement>& stmt);
    HCSValue executeVariableDeclaration(const std::shared_ptr<VariableDeclaration>& stmt);
    HCSValue executeConstantDeclaration(const std::shared_ptr<ConstantDeclaration>& stmt);
    HCSValue executeAssignment(const std::shared_ptr<Assignment>& stmt);
    HCSValue executeBlockStatement(const std::shared_ptr<BlockStatement>& stmt);
    HCSValue executeIfStatement(const std::shared_ptr<IfStatement>& stmt);
    HCSValue executeForStatement(const std::shared_ptr<ForStatement>& stmt);
    HCSValue executeDisplayStatement(const std::shared_ptr<DisplayStatement>& stmt);
    
    // Expression execution
    HCSValue executeLiteralExpression(const std::shared_ptr<LiteralExpression>& expr);
    HCSValue executeIdentifierExpression(const std::shared_ptr<IdentifierExpression>& expr);
    HCSValue executeBinaryExpression(const std::shared_ptr<BinaryExpression>& expr);
    HCSValue executeUnaryExpression(const std::shared_ptr<UnaryExpression>& expr);
    HCSValue executeCallExpression(const std::shared_ptr<CallExpression>& expr);
    
    // Helper functions
    void setVariable(const std::string& name, const HCSValue& value);
    HCSValue getVariable(const std::string& name) const;
    bool isConstant(const std::string& name) const;
};
