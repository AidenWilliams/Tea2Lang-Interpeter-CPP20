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

namespace semantic {
    class ReturnsException : public std::exception {
        [[nodiscard]] const char *what() const noexcept override {
            return "Returns Design Exception";
        }
    };

    class VariableTypeException : public std::exception {
        [[nodiscard]] const char *what() const noexcept override {
            return "Variable Has No Type Design Exception";
        }
    };

    class FunctionTypeException : public std::exception {
        [[nodiscard]] const char *what() const noexcept override {
            return "Function Has No Type Design Exception";
        }
    };
    class StructInsertionException : public std::exception{
        [[nodiscard]] const char* what() const noexcept override{
            return "Insertion of variable/function into struct failed.";
        }
    };


    class Variable {
    public:
        explicit Variable(std::string identifier) :
                type(""),
                identifier(std::move(identifier)),
                lineNumber(0) {};

        Variable(std::string type, std::string identifier, bool array, unsigned int lineNumber) :
                type(std::move(type)),
                identifier(std::move(identifier)),
                array(array),
                lineNumber(lineNumber)
        {};
        ~Variable() = default;

        std::string type;
        std::string identifier;
        bool array{};
        unsigned int lineNumber;
    };

    class Function {
    public:
        Function(std::string identifier, std::vector<std::string> paramTypes) :
                type(""),
                identifier(std::move(identifier)),
                paramTypes(std::move(paramTypes)),
                lineNumber(0)
        {};
        Function(std::string type, std::string identifier, std::vector<std::string> paramTypes, unsigned int lineNumber) :
                type(std::move(type)),
                identifier(std::move(identifier)),
                paramTypes(std::move(paramTypes)),
                lineNumber(lineNumber)
        {};
        ~Function() = default;

        std::string identifier;
        std::vector<std::string> paramTypes;
        unsigned int lineNumber;
        std::string type;
    };

    class Struct{
    public:
        Struct(std::string identifier) :
                identifier(std::move(identifier)),
                variables(std::vector<Variable>()),
                functions(std::vector<Function>()),
                lineNumber()
        {};
        ~Struct() = default;

        std::string identifier;
        std::vector<Variable> variables;
        std::vector<Function> functions;
        unsigned int lineNumber;

        void insert(const Variable& v);
        void insert(const Function& f);
        void defineLineNumber(unsigned int lineNumber);
    };

    class Scope {
    private:
        // Python equivalent of:
        // variableTable = {identifier: {TYPE, identifier, lineNumber}}
        std::map<std::string, Variable> variableTable;
        // Python equivalent of:
        // functionTable = {{identifier, [ARGUMENT_TYPES,]}: {TYPE, identifier, [ARGUMENT_TYPES,], lineNumber}}
        std::map<std::pair<std::string, std::vector<std::string>>, Function> functionTable;
        // Python equivalent of:
        // StructTable = {{identifier}: {identifier, [variables], [functions], lineNumber}}
        std::map<std::string, Struct> structTable;
        bool functionDeclarable;
    public:
        explicit Scope(bool functionDeclarable=false) : functionDeclarable(functionDeclarable) {};
        ~Scope() = default;

        [[nodiscard]] bool isFunctionDeclarable() const { return functionDeclarable; }

        bool insert(const Variable& v);
        bool insert(const Function& f);
        bool insert(const Struct& s);

        auto find(const Variable& v);
        auto find(const Function& f);
        auto find(const Struct& s);

        void insertTo(const Struct& s, const Variable& v);
        void insertTo(const Struct& s, const Function& f);

        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Variable>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::pair<std::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>, Function>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Struct>> result);

        bool erase(std::_Rb_tree_iterator<std::pair<const std::pair<std::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>, Function>> result);
    };
}


namespace visitor {
    class SemanticAnalyser : public Visitor {
    public:
        SemanticAnalyser()
        {
            currentType = std::string();
            structID = std::string();
            returns = false;
            structScope = std::shared_ptr<semantic::Scope>();
        };
        ~SemanticAnalyser() = default;

        std::vector<std::shared_ptr<semantic::Scope>> scopes;
        std::string currentType;
        std::string structID;
        std::shared_ptr<semantic::Scope> structScope;
        bool returns;

        void visit(parser::ASTProgramNode* programNode) override;

        void visit(parser::ASTLiteralNode<int>* literalNode) override;
        void visit(parser::ASTLiteralNode<float>* literalNode) override;
        void visit(parser::ASTLiteralNode<bool>* literalNode) override;
        void visit(parser::ASTLiteralNode<char>* literalNode) override;
        void visit(parser::ASTLiteralNode<std::string>* literalNode) override;
        void visit(parser::ASTArrayLiteralNode* arrayLiteralNode) override;
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
        void visit(parser::ASTStructNode* structNode) override;
    };
}

#endif //TEALANG_COMPILER_CPP20_SEMANTIC_VISITOR_H