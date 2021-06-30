//
// Created by Aiden Williams on 26/06/2021.
//

#ifndef TEALANG_COMPILER_CPP20_SEMANTIC_VISITOR_H
#define TEALANG_COMPILER_CPP20_SEMANTIC_VISITOR_H

#include "Visitor.h"
#include "../Parser/AST.h"
#include <utility>
#include <vector>
#include <map>
#include <memory>
#include "../Lexer/Token.h"

namespace visitor {
    class ReturnsException : public std::exception{
        [[nodiscard]] const char* what() const noexcept override{
            return "Returns Design Exception";
        }
    };

    class Variable{
    public:
        Variable(std::string identifier, unsigned int lineNumber) :
                identifier(std::move(identifier)),
                lineNumber(lineNumber)
        {};
        Variable(std::string type, std::string identifier, unsigned int lineNumber) :
                type(std::move(type)),
                identifier(std::move(identifier)),
                lineNumber(lineNumber)
        {};
        ~Variable() = default;

        std::string identifier;
        std::string type;
        unsigned int lineNumber;
    };

    class Function{
    public:
        Function(std::string identifier, std::vector<std::string> paramTypes, unsigned int lineNumber) :
                identifier(std::move(identifier)),
                paramTypes(std::move(paramTypes)),
                lineNumber(lineNumber)
        {};

        Function(std::string identifier,std::string type, std::vector<std::string> paramTypes, unsigned int lineNumber) :
                identifier(std::move(identifier)),
                type(std::move(type)),
                paramTypes(std::move(paramTypes)),
                lineNumber(lineNumber)
        {};
        ~Function() = default;

        std::string identifier;
        std::string type;
        std::vector<std::string> paramTypes;
        unsigned int lineNumber;
    };

    class SemanticScope {
    private:
        // Python equivalent of:
        // variableTable = {identifier: {TYPE, identifier, lineNumber}}
        std::map<std::string, Variable> variableTable;
        // Python equivalent of:
        // functionTable = {identifier: { identifier, [ARGUMENT_TYPES,], lineNumber}}
        std::map<std::string, Function> functionTable;
        bool global;
    public:
        explicit SemanticScope(bool global=false) : global(global) {};
        ~SemanticScope() = default;

        [[nodiscard]] bool isGlobal() const { return global; }

        bool insert(const Variable& v);
        bool insert(const Function& f);

        std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Variable>>
        find(const Variable& v);
        std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Function>>
        find(const Function& f);

        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Function>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Variable>> result);
    };


    class SemanticAnalyser : public Visitor {
    public:
        SemanticAnalyser()
        {
            scopes.emplace_back(std::make_shared<SemanticScope>());
            currentType = std::string();
            returns = false;
        };
        ~SemanticAnalyser() = default;

        std::vector<std::shared_ptr<SemanticScope>> scopes;
        std::string currentType;
        bool returns;

        void visit(parser::ASTProgramNode* programNode) override;

        void visit(parser::ASTLiteralNode<int>* literalNode) override;
        void visit(parser::ASTLiteralNode<float>* literalNode) override;
        void visit(parser::ASTLiteralNode<bool>* literalNode) override;
        void visit(parser::ASTLiteralNode<std::string>* literalNode) override;
        void visit(parser::ASTBinaryNode* binaryNode) override;
        void visit(parser::ASTIdentifierNode* identifierNode) override;
        void visit(parser::ASTUnaryNode* unaryNode) override;
        void visit(parser::ASTFunctionCallNode* functionCallNode) override;

        void visit(parser::ASTSFunctionCallNode* sFunctionCallNode) override;
        void visit(parser::ASTDeclarationNode* declarationNode) override;
        void visit(parser::ASTAssignmentNode* assignmentNode) override;
        void visit(parser::ASTPrintNode* printNode) override;
        void visit(parser::ASTBlockNode* blockNode) override;
        void visit(parser::ASTIfNode* ifNode) override;
        void visit(parser::ASTForNode* forNode) override;
        void visit(parser::ASTWhileNode* whileNode) override;
        void visit(parser::ASTFunctionDeclarationNode* functionDeclarationNode) override;
        void visit(parser::ASTReturnNode* returnNode) override;
    };
}



#endif //TEALANG_COMPILER_CPP20_SEMANTIC_VISITOR_H
