//
// Created by Aiden Williams on 03/05/2021.
//

#ifndef TEALANG_COMPILER_CPP20_AST_H
#define TEALANG_COMPILER_CPP20_AST_H


#include <string>
#include <vector>
#include "../Visitor/Visitor.h"

namespace parser {
    // Types
    enum TYPE {FLOAT, INT, BOOL, STRING};

    // Abstract Nodes
    class ASTNode {
    public:
        ASTNode() = default;
        ~ASTNode() = default;
        virtual void accept(visitor::Visitor*) = 0;
    };

    class ASTStatementNode : public ASTNode {
    public:
        ASTStatementNode() = default;
        ~ASTStatementNode() = default;
        void accept(visitor::Visitor*) override = 0;
    };
    // Expression Nodes
    class ASTExprNode : public ASTNode {
    public:
        ASTExprNode() = default;
        ~ASTExprNode() = default;
        void accept(visitor::Visitor*) override = 0;
    };

    template <typename T>
    class ASTLiteralNode : public ASTExprNode {
    public:
        ASTLiteralNode(T val, unsigned int lineNumber) :
            val(val),
            lineNumber(lineNumber)
            {};
        ~ASTLiteralNode() = default;
        T val;
        unsigned int lineNumber;
        void accept(visitor::Visitor*) override;
    };

    class ASTBinaryExprNode : public ASTExprNode {
    public:
        ASTBinaryExprNode(std::string op, ASTExprNode* left, ASTExprNode* right, unsigned int lineNumber) :
                op(std::move(op)),
                left(left),
                right(right),
                lineNumber(lineNumber)
                {};
        ~ASTBinaryExprNode() = default;;
        std::string op;
        ASTExprNode *left;
        ASTExprNode *right;
        unsigned int lineNumber;
        void accept(visitor::Visitor*) override;
    };

    class ASTIdentifierNode : public ASTExprNode {
    public:
        explicit ASTIdentifierNode(std::string identifier, unsigned int lineNumber) :
                identifier(identifier),
                lineNumber(lineNumber)
                {};
        std::string identifier;
        unsigned int lineNumber;
        void accept(visitor::Visitor*) override;
    };

    class ASTUnaryExprNode : public ASTExprNode {
    public:
        ASTUnaryExprNode(std::string op, ASTExprNode* expr, unsigned int lineNumber) :
                op(std::move(op)),
                expr(expr),
                lineNumber(lineNumber)
        {};
        std::string op;
        ASTExprNode *expr;
        unsigned int lineNumber;
        void accept(visitor::Visitor*) override;
    };

    class ASTFunctionCallNode : public ASTExprNode {
    public:
        ASTFunctionCallNode(std::string identifier, std::vector<ASTExprNode*> parameters, unsigned int lineNumber) :
                identifier(std::move(identifier)),
                parameters(std::move(parameters)),
                lineNumber(lineNumber)
                {};
        std::string identifier;
        std::vector<ASTExprNode*> parameters;
        unsigned int lineNumber;
        void accept(visitor::Visitor*) override;
    };
}

}

#endif //TEALANG_COMPILER_CPP20_AST_H
