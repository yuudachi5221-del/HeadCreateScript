#include "interpreter.h"
#include <iostream>
#include <cmath>

Interpreter::Interpreter() = default;

void Interpreter::interpret(const std::shared_ptr<Program>& program) {
    if (!program) return;
    
    for (const auto& stmt : program->statements) {
        try {
            executeStatement(stmt);
        } catch (const std::runtime_error& e) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }
    }
}

// ============================================================================
// Statement Execution
// ============================================================================

HCSValue Interpreter::executeStatement(const StmtPtr& stmt) {
    if (!stmt) return HCSValue();
    
    if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt)) {
        return executeExpressionStatement(exprStmt);
    }
    if (auto varDecl = std::dynamic_pointer_cast<VariableDeclaration>(stmt)) {
        return executeVariableDeclaration(varDecl);
    }
    if (auto constDecl = std::dynamic_pointer_cast<ConstantDeclaration>(stmt)) {
        return executeConstantDeclaration(constDecl);
    }
    if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        return executeAssignment(assign);
    }
    if (auto block = std::dynamic_pointer_cast<BlockStatement>(stmt)) {
        return executeBlockStatement(block);
    }
    if (auto ifStmt = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        return executeIfStatement(ifStmt);
    }
    if (auto forStmt = std::dynamic_pointer_cast<ForStatement>(stmt)) {
        return executeForStatement(forStmt);
    }
    if (auto dispStmt = std::dynamic_pointer_cast<DisplayStatement>(stmt)) {
        return executeDisplayStatement(dispStmt);
    }
    
    return HCSValue();
}

HCSValue Interpreter::executeExpressionStatement(const std::shared_ptr<ExpressionStatement>& stmt) {
    if (stmt->expr) return executeExpression(stmt->expr);
    return HCSValue();
}

HCSValue Interpreter::executeVariableDeclaration(const std::shared_ptr<VariableDeclaration>& stmt) {
    HCSValue value;
    if (stmt->initializer) {
        value = executeExpression(stmt->initializer);
    } else {
        value = HCSValue::blank();
    }
    setVariable(stmt->name, value);
    return HCSValue();
}

HCSValue Interpreter::executeConstantDeclaration(const std::shared_ptr<ConstantDeclaration>& stmt) {
    HCSValue value = executeExpression(stmt->initializer);
    setVariable(stmt->name, value);
    constantFlags_[stmt->name] = true;
    return HCSValue();
}

HCSValue Interpreter::executeAssignment(const std::shared_ptr<Assignment>& stmt) {
    if (isConstant(stmt->name)) {
        throw std::runtime_error("Cannot assign to constant '" + stmt->name + "'");
    }
    HCSValue value = executeExpression(stmt->value);
    setVariable(stmt->name, value);
    return HCSValue();
}

HCSValue Interpreter::executeBlockStatement(const std::shared_ptr<BlockStatement>& stmt) {
    for (const auto& s : stmt->statements) {
        executeStatement(s);
    }
    return HCSValue();
}

HCSValue Interpreter::executeIfStatement(const std::shared_ptr<IfStatement>& stmt) {
    HCSValue condition = executeExpression(stmt->condition);
    
    if (condition.toBoolean()) {
        return executeStatement(stmt->thenBranch);
    } else if (stmt->elseBranch) {
        return executeStatement(stmt->elseBranch);
    }
    return HCSValue();
}

HCSValue Interpreter::executeForStatement(const std::shared_ptr<ForStatement>& stmt) {
    HCSValue start = executeExpression(stmt->start);
    HCSValue end = executeExpression(stmt->end);
    HCSValue step = executeExpression(stmt->step);
    
    long long current = start.toInteger();
    long long endVal = end.toInteger();
    long long stepVal = step.toInteger();
    
    if (stepVal == 0) {
        throw std::runtime_error("For loop step cannot be zero");
    }
    
    // Determine if we're looping up or down
    bool loopUp = (stmt->endOp == "<=" || stmt->endOp == "<" || 
                   stmt->endOp == "==" || stmt->endOp == ">=");
    
    // Execute loop
    if (stepVal > 0) {
        while ((loopUp && current <= endVal) || (!loopUp && current >= endVal)) {
            setVariable(stmt->varName, HCSValue(current));
            executeStatement(stmt->body);
            current += stepVal;
        }
    } else {
        while ((loopUp && current >= endVal) || (!loopUp && current <= endVal)) {
            setVariable(stmt->varName, HCSValue(current));
            executeStatement(stmt->body);
            current += stepVal;
        }
    }
    
    return HCSValue();
}

HCSValue Interpreter::executeDisplayStatement(const std::shared_ptr<DisplayStatement>& stmt) {
    HCSValue value = executeExpression(stmt->value);
    std::cout << value.toString() << std::endl;
    return HCSValue();
}

// ============================================================================
// Expression Execution
// ============================================================================

HCSValue Interpreter::executeExpression(const ExprPtr& expr) {
    if (!expr) return HCSValue();
    
    if (auto literal = std::dynamic_pointer_cast<LiteralExpression>(expr)) {
        return executeLiteralExpression(literal);
    }
    if (auto ident = std::dynamic_pointer_cast<IdentifierExpression>(expr)) {
        return executeIdentifierExpression(ident);
    }
    if (auto binary = std::dynamic_pointer_cast<BinaryExpression>(expr)) {
        return executeBinaryExpression(binary);
    }
    if (auto unary = std::dynamic_pointer_cast<UnaryExpression>(expr)) {
        return executeUnaryExpression(unary);
    }
    if (auto call = std::dynamic_pointer_cast<CallExpression>(expr)) {
        return executeCallExpression(call);
    }
    
    return HCSValue();
}

HCSValue Interpreter::executeLiteralExpression(const std::shared_ptr<LiteralExpression>& expr) {
    switch (expr->type) {
        case LiteralExpression::Type::Integer:
            return HCSValue(std::stoll(expr->value));
        case LiteralExpression::Type::Double:
            return HCSValue(std::stod(expr->value));
        case LiteralExpression::Type::String:
            return HCSValue(expr->value);
        case LiteralExpression::Type::Bool:
            return HCSValue(expr->value == "true");
        case LiteralExpression::Type::Blank:
            return HCSValue::blank();
    }
    return HCSValue();
}

HCSValue Interpreter::executeIdentifierExpression(const std::shared_ptr<IdentifierExpression>& expr) {
    return getVariable(expr->name);
}

HCSValue Interpreter::executeBinaryExpression(const std::shared_ptr<BinaryExpression>& expr) {
    HCSValue left = executeExpression(expr->left);
    HCSValue right = executeExpression(expr->right);
    
    switch (expr->op) {
        case BinaryExpression::Op::Add:
            return left + right;
        case BinaryExpression::Op::Sub:
            return left - right;
        case BinaryExpression::Op::Mul:
            return left * right;
        case BinaryExpression::Op::Div:
            return left / right;
        case BinaryExpression::Op::Mod:
            return left % right;
        case BinaryExpression::Op::Eq:
            return HCSValue(left == right);
        case BinaryExpression::Op::Ne:
            return HCSValue(left != right);
        case BinaryExpression::Op::Lt:
            return HCSValue(left < right);
        case BinaryExpression::Op::Gt:
            return HCSValue(left > right);
        case BinaryExpression::Op::Lte:
            return HCSValue(left <= right);
        case BinaryExpression::Op::Gte:
            return HCSValue(left >= right);
    }
    
    return HCSValue();
}

HCSValue Interpreter::executeUnaryExpression(const std::shared_ptr<UnaryExpression>& expr) {
    HCSValue operand = executeExpression(expr->operand);
    
    switch (expr->op) {
        case UnaryExpression::Op::Neg:
            return HCSValue(-operand.toInteger());
        case UnaryExpression::Op::Not:
            return HCSValue(!operand.toBoolean());
    }
    
    return HCSValue();
}

HCSValue Interpreter::executeCallExpression(const std::shared_ptr<CallExpression>& expr) {
    // Built-in functions could be handled here
    throw std::runtime_error("Function '" + expr->name + "' is not defined");
}

// ============================================================================
// Helper Functions
// ============================================================================

void Interpreter::setVariable(const std::string& name, const HCSValue& value) {
    globalVariables_[name] = value;
}

HCSValue Interpreter::getVariable(const std::string& name) const {
    auto it = globalVariables_.find(name);
    if (it == globalVariables_.end()) {
        throw std::runtime_error("Undefined variable '" + name + "'");
    }
    return it->second;
}

bool Interpreter::isConstant(const std::string& name) const {
    auto it = constantFlags_.find(name);
    return it != constantFlags_.end() && it->second;
}
