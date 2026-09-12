#pragma once

#include <string>
#include <unordered_map>
#include "token.h"

class TokenUtility {
public:
    // Get human-readable name for token type
    static std::string getTokenTypeName(TokenType type) {
        static const std::unordered_map<int, std::string> typeNames = {
            {static_cast<int>(TokenType::EOF_), "EOF"},
            {static_cast<int>(TokenType::Identifier), "Identifier"},
            {static_cast<int>(TokenType::Number), "Number"},
            {static_cast<int>(TokenType::String), "String"},
            {static_cast<int>(TokenType::Bool), "Bool"},
            {static_cast<int>(TokenType::Keyword), "Keyword"},
            {static_cast<int>(TokenType::Symbol), "Symbol"},
            {static_cast<int>(TokenType::Plus), "Plus"},
            {static_cast<int>(TokenType::Minus), "Minus"},
            {static_cast<int>(TokenType::Star), "Star"},
            {static_cast<int>(TokenType::Slash), "Slash"},
            {static_cast<int>(TokenType::Percent), "Percent"},
            {static_cast<int>(TokenType::Assign), "Assign"},
            {static_cast<int>(TokenType::Equal), "Equal"},
            {static_cast<int>(TokenType::NotEqual), "NotEqual"},
            {static_cast<int>(TokenType::Less), "Less"},
            {static_cast<int>(TokenType::Greater), "Greater"},
            {static_cast<int>(TokenType::LessEq), "LessEq"},
            {static_cast<int>(TokenType::GreaterEq), "GreaterEq"},
            {static_cast<int>(TokenType::Arrow), "Arrow"},
            {static_cast<int>(TokenType::LParen), "LParen"},
            {static_cast<int>(TokenType::RParen), "RParen"},
            {static_cast<int>(TokenType::LBrace), "LBrace"},
            {static_cast<int>(TokenType::RBrace), "RBrace"},
            {static_cast<int>(TokenType::Semicolon), "Semicolon"},
            {static_cast<int>(TokenType::Unknown), "Unknown"}
        };
        
        auto it = typeNames.find(static_cast<int>(type));
        return (it != typeNames.end()) ? it->second : "Unknown";
    }
    
    // Check if token is a keyword
    static bool isKeyword(const Token& token) {
        return token.type == TokenType::Keyword;
    }
    
    // Check if token is an operator
    static bool isOperator(const Token& token) {
        return token.type == TokenType::Plus || token.type == TokenType::Minus ||
               token.type == TokenType::Star || token.type == TokenType::Slash ||
               token.type == TokenType::Percent || token.type == TokenType::Assign ||
               token.type == TokenType::Equal || token.type == TokenType::NotEqual ||
               token.type == TokenType::Less || token.type == TokenType::Greater ||
               token.type == TokenType::LessEq || token.type == TokenType::GreaterEq ||
               token.type == TokenType::Arrow;
    }
    
    // Check if token is a binary operator
    static bool isBinaryOperator(const Token& token) {
        return token.type == TokenType::Plus || token.type == TokenType::Minus ||
               token.type == TokenType::Star || token.type == TokenType::Slash ||
               token.type == TokenType::Percent || token.type == TokenType::Equal ||
               token.type == TokenType::NotEqual || token.type == TokenType::Less ||
               token.type == TokenType::Greater || token.type == TokenType::LessEq ||
               token.type == TokenType::GreaterEq;
    }
    
    // Check if token is an assignment operator
    static bool isAssignmentOperator(const Token& token) {
        return token.type == TokenType::Assign;
    }
    
    // Check if token is a comparison operator
    static bool isComparisonOperator(const Token& token) {
        return token.type == TokenType::Equal || token.type == TokenType::NotEqual ||
               token.type == TokenType::Less || token.type == TokenType::Greater ||
               token.type == TokenType::LessEq || token.type == TokenType::GreaterEq;
    }
    
    // Check if token is a delimiter
    static bool isDelimiter(const Token& token) {
        return token.type == TokenType::LParen || token.type == TokenType::RParen ||
               token.type == TokenType::LBrace || token.type == TokenType::RBrace ||
               token.type == TokenType::Semicolon;
    }
    
    // Check if token is a literal
    static bool isLiteral(const Token& token) {
        return token.type == TokenType::Number || token.type == TokenType::String ||
               token.type == TokenType::Bool;
    }
    
    // Get operator precedence (higher number = higher precedence)
    static int getOperatorPrecedence(const Token& token) {
        switch (token.type) {
            case TokenType::Star:
            case TokenType::Slash:
            case TokenType::Percent:
                return 60;  // Multiplication/Division
            
            case TokenType::Plus:
            case TokenType::Minus:
                return 50;  // Addition/Subtraction
            
            case TokenType::Less:
            case TokenType::Greater:
            case TokenType::LessEq:
            case TokenType::GreaterEq:
                return 40;  // Comparison
            
            case TokenType::Equal:
            case TokenType::NotEqual:
                return 35;  // Equality
            
            case TokenType::Assign:
                return 10;  // Assignment (lowest)
            
            default:
                return 0;   // Not an operator
        }
    }
    
    // Check if operator is right-associative
    static bool isRightAssociative(const Token& token) {
        return token.type == TokenType::Assign;
    }
    
    // Format token for display
    static std::string formatToken(const Token& token) {
        std::string result = getTokenTypeName(token.type) + "(";
        
        if (!token.text.empty()) {
            result += token.text;
        }
        
        if (!token.value.empty() && token.value != token.text) {
            result += " [" + token.value + "]";
        }
        
        result += ")";
        return result;
    }
    
    // Format token with position
    static std::string formatTokenWithLocation(const Token& token) {
        return formatToken(token) + " at " + std::to_string(token.line) + ":" + 
               std::to_string(token.col);
    }
};
