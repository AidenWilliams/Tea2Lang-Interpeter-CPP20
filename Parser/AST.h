//
// Created by Aiden Williams on 03/05/2021.
//

#ifndef TEALANG_COMPILER_CPP20_AST_H
#define TEALANG_COMPILER_CPP20_AST_H

#include <string>
#include <utility>
#include <vector>
#include <memory>
#include "../Visitor/Visitor.h"

namespace parser {
    // Abstract Nodes
    class ASTNode {
    public:
        ASTNode() = default;
        ~ASTNode() = default;
        virtual void accept(visitor::Visitor* v) = 0;
    };

    // Expression Nodes
    class ASTExprNode : public ASTNode {
    public:
        ASTExprNode() = default;
        ~ASTExprNode() = default;
        void accept(visitor::Visitor* v) override = 0;
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
        void accept(visitor::Visitor* v) override;
    };

    class ASTBinaryNode : public ASTExprNode {
    public:
        ASTBinaryNode(std::string op, std::shared_ptr<ASTExprNode> left, std::shared_ptr<ASTExprNode> right, unsigned int lineNumber) :
                op(std::move(op)),
                left(std::move(left)),
                right(std::move(right)),
                lineNumber(lineNumber)
        {};
        std::string op;
        std::shared_ptr<ASTExprNode> left;
        std::shared_ptr<ASTExprNode> right;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTFunctionCallNode : public ASTExprNode {
    public:
        ASTFunctionCallNode(std::shared_ptr<ASTIdentifierNode> identifier, std::vector<std::shared_ptr<ASTExprNode>> parameters, unsigned int lineNumber) :
                identifier(std::move(identifier)),
                parameters(std::move(parameters)),
                lineNumber(lineNumber)
        {};
        ~ASTFunctionCallNode() = default;

        std::shared_ptr<ASTIdentifierNode> identifier;
        std::vector<std::shared_ptr<ASTExprNode>> parameters;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTIdentifierNode : public ASTExprNode {
    public:
        explicit ASTIdentifierNode(std::string identifier, unsigned int lineNumber) :
                identifier(std::move(identifier)),
                lineNumber(lineNumber)
        {};
        ~ASTIdentifierNode() = default;

        std::string identifier;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTUnaryNode : public ASTExprNode {
    public:
        explicit ASTUnaryNode(std::shared_ptr<ASTExprNode> exprNode, std::string op, unsigned int lineNumber) :
            exprNode(std::move(exprNode)),
            op(std::move(op)),
            lineNumber(lineNumber)
        {};
        ~ASTUnaryNode() = default;
        std::shared_ptr<ASTExprNode> exprNode;
        std::string op;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    // Statement Nodes
    class ASTStatementNode : public ASTNode {
    public:
        ASTStatementNode() = default;
        ~ASTStatementNode() = default;
        void accept(visitor::Visitor* v) override = 0;
    };

    // Program Node
    class ASTProgramNode : public ASTNode {
    public:
        explicit ASTProgramNode(std::vector<std::shared_ptr<ASTStatementNode>> statements) :
                statements(std::move(statements))
        {};

        explicit ASTProgramNode(const std::shared_ptr<ASTProgramNode>& programNode) :
                statements(std::move(programNode->statements))
        {};

        ~ASTProgramNode() = default;

        std::vector<std::shared_ptr<ASTStatementNode>> statements;
        void accept(visitor::Visitor* v) override;
    };

    class ASTSFunctionCallNode : public ASTStatementNode {
        // Identical to the expression node but is a statement
    public:
        ASTSFunctionCallNode(std::shared_ptr<ASTIdentifierNode> identifier, std::vector<std::shared_ptr<ASTExprNode>> parameters, unsigned int lineNumber) :
                identifier(std::move(identifier)),
                parameters(std::move(parameters)),
                lineNumber(lineNumber)
        {};

        explicit ASTSFunctionCallNode(const std::shared_ptr<ASTFunctionCallNode>& exprNode) :
                identifier(exprNode->identifier),
                parameters(exprNode->parameters),
                lineNumber(exprNode->lineNumber)
        {};

        ~ASTSFunctionCallNode() = default;

        std::shared_ptr<ASTIdentifierNode> identifier;
        std::vector<std::shared_ptr<ASTExprNode>> parameters;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTDeclarationNode : public ASTStatementNode {
    public:
        ASTDeclarationNode(std::string type, std::shared_ptr<ASTIdentifierNode> identifier, std::shared_ptr<ASTExprNode> exprNode, unsigned int lineNumber) :
                type(std::move(type)),
                identifier(std::move(identifier)),
                exprNode(std::move(exprNode)),
                lineNumber(lineNumber)
        {};
        ~ASTDeclarationNode() = default;

        std::string type;
        std::shared_ptr<ASTIdentifierNode> identifier;
        std::shared_ptr<ASTExprNode> exprNode;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTAssignmentNode : public ASTStatementNode {
    public:
        ASTAssignmentNode(std::shared_ptr<ASTIdentifierNode> identifier, std::shared_ptr<ASTExprNode> exprNode, unsigned int lineNumber) :
                identifier(std::move(identifier)),
                exprNode(std::move(exprNode)),
                lineNumber(lineNumber)
        {};
        ~ASTAssignmentNode() = default;

        std::shared_ptr<ASTIdentifierNode> identifier;
        std::shared_ptr<ASTExprNode> exprNode;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTPrintNode : public ASTStatementNode {
    public:
        ASTPrintNode(std::shared_ptr<ASTExprNode> exprNode, unsigned int lineNumber) :
                exprNode(std::move(exprNode)),
                lineNumber(lineNumber)
        {};
        ~ASTPrintNode() = default;

        std::shared_ptr<ASTIdentifierNode> identifier;
        std::shared_ptr<ASTExprNode> exprNode;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTBlockNode : public ASTStatementNode {
    public:
        ASTBlockNode(std::vector<std::shared_ptr<ASTStatementNode>> statements, unsigned int lineNumber) :
                statements(std::move(statements)),
                lineNumber(lineNumber)
        {};

        ASTBlockNode(const std::shared_ptr<ASTProgramNode>& program, unsigned int lineNumber) :
                statements(program->statements),
                lineNumber(lineNumber)
        {};

        ~ASTBlockNode() = default;

        std::vector<std::shared_ptr<ASTStatementNode>> statements;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTIfNode : public ASTStatementNode {
    public:
        ASTIfNode(std::shared_ptr<ASTExprNode> condition, std::shared_ptr<ASTBlockNode> ifBlock, unsigned int lineNumber, std::shared_ptr<ASTBlockNode> elseBlock = nullptr) :
                condition(std::move(condition)),
                ifBlock(std::move(ifBlock)),
                elseBlock(std::move(elseBlock)),
                lineNumber(lineNumber)
        {};
        ~ASTIfNode() = default;

        std::shared_ptr<ASTExprNode> condition;
        std::shared_ptr<ASTBlockNode> ifBlock;
        std::shared_ptr<ASTBlockNode> elseBlock;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };
    class ASTForNode : public ASTStatementNode {
    public:
        ASTForNode(std::shared_ptr<ASTExprNode> condition, std::shared_ptr<ASTBlockNode> loopBlock, unsigned int lineNumber,
                std::shared_ptr<ASTDeclarationNode> declaration = nullptr, std::shared_ptr<ASTAssignmentNode> assignment = nullptr) :
                condition(std::move(condition)),
                declaration(std::move(declaration)),
                assignment(std::move(assignment)),
                loopBlock(std::move(loopBlock)),
                lineNumber(lineNumber)
        {};
        ~ASTForNode() = default;

        std::shared_ptr<ASTDeclarationNode> declaration;
        std::shared_ptr<ASTExprNode> condition;
        std::shared_ptr<ASTAssignmentNode> assignment;
        std::shared_ptr<ASTBlockNode> loopBlock;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };


    class ASTWhileNode : public ASTStatementNode {
    public:
        ASTWhileNode(std::shared_ptr<ASTExprNode> condition, std::shared_ptr<ASTBlockNode> loopBlock, unsigned int lineNumber) :
                condition(std::move(condition)),
                loopBlock(std::move(loopBlock)),
                lineNumber(lineNumber)
        {};
        ~ASTWhileNode() = default;

        std::shared_ptr<ASTExprNode> condition;
        std::shared_ptr<ASTBlockNode> loopBlock;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTFunctionDeclarationNode : public ASTStatementNode {
    public:
        ASTFunctionDeclarationNode(std::string type, std::shared_ptr<ASTIdentifierNode> identifier, std::vector<std::pair<std::string, std::string>> parameters,
                                   std::shared_ptr<ASTBlockNode> functionBlock, unsigned int lineNumber) :
                type(std::move(type)),
                identifier(std::move(identifier)),
                parameters(std::move(parameters)),
                functionBlock(std::move(functionBlock)),
                lineNumber(lineNumber)
        {};
        ~ASTFunctionDeclarationNode() = default;

        std::string type;
        std::shared_ptr<ASTIdentifierNode> identifier;
        //                  Type Identifier
        std::vector<std::pair<std::string, std::string>> parameters;
        std::shared_ptr<ASTBlockNode> functionBlock;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };

    class ASTReturnNode : public ASTStatementNode {
    public:
        ASTReturnNode(std::shared_ptr<ASTExprNode> exprNode, unsigned int lineNumber) :
            exprNode(std::move(exprNode)),
            lineNumber(lineNumber)
        {};
        ~ASTReturnNode() = default;

        std::shared_ptr<ASTExprNode> exprNode;
        unsigned int lineNumber;
        void accept(visitor::Visitor* v) override;
    };
}
#endif //TEALANG_COMPILER_CPP20_AST_H
