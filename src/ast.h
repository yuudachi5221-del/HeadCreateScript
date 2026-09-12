#pragma once

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include "token.h"

// Forward declarations
class ASTNode;
class Expression;
class Statement;
class Program;

using ASTNodePtr = std::shared_ptr<ASTNode>;
using ExprPtr = std::shared_ptr<Expression>;
using StmtPtr = std::shared_ptr<Statement>;

// ============================================================================
// Base Classes
// ============================================================================

class ASTNode {
public:
    virtual ~ASTNode() = default;
    int line = 0;
    int col = 0;
};

// ============================================================================
// Expressions
// ============================================================================

class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

class LiteralExpression : public Expression {
public:
    enum class Type { Integer, Double, String, Bool, Blank };
    Type type;
    std::string value;  // stored as string, parsed at runtime
    
    LiteralExpression(Type t, const std::string& v) : type(t), value(v) {}
};

class IdentifierExpression : public Expression {
public:
    std::string name;
    
    explicit IdentifierExpression(const std::string& n) : name(n) {}
};

class BinaryExpression : public Expression {
public:
    enum class Op { Add, Sub, Mul, Div, Mod, Eq, Ne, Lt, Gt, Lte, Gte };
    Op op;
    ExprPtr left;
    ExprPtr right;
    
    BinaryExpression(Op o, ExprPtr l, ExprPtr r) : op(o), left(l), right(r) {}
};

class UnaryExpression : public Expression {
public:
    enum class Op { Neg, Not };
    Op op;
    ExprPtr operand;
    
    UnaryExpression(Op o, ExprPtr expr) : op(o), operand(expr) {}
};

class CallExpression : public Expression {
public:
    std::string name;
    std::vector<ExprPtr> arguments;
    
    explicit CallExpression(const std::string& n) : name(n) {}
};

// ============================================================================
// Statements
// ============================================================================

class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class ExpressionStatement : public Statement {
public:
    ExprPtr expr;
    
    explicit ExpressionStatement(ExprPtr e) : expr(e) {}
};

class VariableDeclaration : public Statement {
public:
    std::string name;
    ExprPtr initializer;  // can be nullptr
    
    VariableDeclaration(const std::string& n, ExprPtr init = nullptr)
        : name(n), initializer(init) {}
};

class ConstantDeclaration : public Statement {
public:
    std::string name;
    ExprPtr initializer;
    
    ConstantDeclaration(const std::string& n, ExprPtr init)
        : name(n), initializer(init) {}
};

class Assignment : public Statement {
public:
    std::string name;
    ExprPtr value;
    
    Assignment(const std::string& n, ExprPtr v) : name(n), value(v) {}
};

class BlockStatement : public Statement {
public:
    std::vector<StmtPtr> statements;
};

class IfStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;  // can be nullptr
    
    IfStatement(ExprPtr cond, StmtPtr then, StmtPtr els = nullptr)
        : condition(cond), thenBranch(then), elseBranch(els) {}
};

class ForStatement : public Statement {
public:
    ExprPtr start;
    std::string varName;
    ExprPtr end;
    std::string endOp;  // "<=", ">=", "<", ">", "==", "!="
    ExprPtr step;       // range value
    StmtPtr body;
    
    ForStatement(ExprPtr s, const std::string& var, ExprPtr e, 
                 const std::string& op, ExprPtr st, StmtPtr b)
        : start(s), varName(var), end(e), endOp(op), step(st), body(b) {}
};

class DisplayStatement : public Statement {
public:
    ExprPtr value;
    
    explicit DisplayStatement(ExprPtr v) : value(v) {}
};

// ============================================================================
// Program
// ============================================================================

class Program : public ASTNode {
public:
    std::vector<StmtPtr> statements;
};
