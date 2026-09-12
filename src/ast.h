#pragma once

#include <memory>
#include <vector>
#include <string>

struct ASTNode {
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

struct ProgramNode : ASTNode {
    std::vector<ASTNodePtr> children;
};

// Further node types (VariableDeclaration, IfStatement, etc.) will be added later.
