//
// Created by aiden on 30/06/2021.
//

#ifndef TEALANG_COMPILER_CPP20_INTERPRETER_VISITOR_H
#define TEALANG_COMPILER_CPP20_INTERPRETER_VISITOR_H

#include "Visitor.h"
#include "Semantic_Visitor.h"
#include <utility>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

namespace interpreter {
    template <typename T>
    class Variable : public visitor::Variable{
    public:
        Variable(const std::string& type, const std::string& identifier, bool array, T value, unsigned int lineNumber) :
                visitor::Variable(type, identifier, array, lineNumber),
                latestValue(value)
                {
                    values.emplace_back(value);
                };

        explicit Variable(const std::string& identifier) :
                visitor::Variable(identifier)
        {};

        Variable(Variable const &v) :
                visitor::Variable(v.type, v.identifier, v.array, v.lineNumber),
                latestValue(v.latestValue),
                values(v.values)
        {};

        ~Variable() = default;
        T latestValue;
        std::vector<T> values;
    };

    class Function : public visitor::Function{
    public:
        Function(const std::string& type, const std::string& identifier, const std::vector<std::string>& paramTypes,
                 std::vector<std::string>  paramIDs,
                 std::shared_ptr<parser::ASTBlockNode> blockNode, unsigned int lineNumber)
                 :
                 visitor::Function(type, identifier, paramTypes, lineNumber),
                 paramIDs(std::move(paramIDs)),
                 blockNode(std::move(blockNode))
                 {};

        explicit Function(const std::string& identifier, const std::vector<std::string>& paramTypes) :
                visitor::Function(identifier, paramTypes)
        {};

        Function(Function const &f) :
                visitor::Function(f.type, f.identifier, f.paramTypes, f.lineNumber),
                paramIDs(f.paramIDs),
                blockNode(f.blockNode)
        {};

        ~Function() = default;
        std::vector<std::string> paramIDs;
        std::shared_ptr<parser::ASTBlockNode> blockNode;
    };

    template <typename T>
    class Array : public visitor::Variable{
    public:
        Array(const std::string& type, const std::string& identifier, bool array, std::vector<T> value, unsigned int lineNumber) :
                visitor::Variable(type, identifier, array, lineNumber),
                latestValue(value)
        {
            values.emplace_back(value);
        };

        explicit Array(const std::string& identifier) :
                visitor::Variable(identifier)
        {};

        Array(Array const &a) :
                visitor::Variable(a.type, a.identifier, a.array, a.lineNumber),
                latestValue(a.latestValue),
                values(a.values)
        {};

        ~Array() = default;
        std::vector<T> latestValue;
        std::vector<std::vector<T>> values;
    };
}

namespace visitor {
    class Interpreter : public Visitor {
    private:
        // Python equivalent of:
        // variableTable = {identifier: {TYPE, identifier, val, values, lineNumber}}
        std::map<std::string, interpreter::Variable<int>>           intTable;
        std::map<std::string, interpreter::Variable<float>>         floatTable;
        std::map<std::string, interpreter::Variable<bool>>          boolTable;
        std::map<std::string, interpreter::Variable<std::string>>   stringTable;
        std::map<std::string, interpreter::Variable<char>>          charTable;
        std::map<std::string, interpreter::Array<int>>              intArrayTable;
        std::map<std::string, interpreter::Array<float>>            floatArrayTable;
        std::map<std::string, interpreter::Array<bool>>             boolArrayTable;
        std::map<std::string, interpreter::Array<std::string>>      stringArrayTable;
        std::map<std::string, interpreter::Array<char>>             charArrayTable;
        // Python equivalent of:
        // functionTable = {{identifier, [ARGUMENT_TYPES,]}: {TYPE, identifier, [ARGUMENT_TYPES,], lineNumber}}
        std::map<std::pair<std::string, std::vector<std::string>>, interpreter::Function> functionTable;

        // type, identifier
        std::string currentType;
        std::string currentID;
        // function block
        bool function;
        //                      Type, Identifier
        std::vector<std::pair<std::string, std::string>> toPop;
        // array flag
        bool array;
    public:
        Interpreter(){
            // insert the interpreter variables these being the literal and 0CurrentVariable for each type
            insert(interpreter::Variable<int>("int", "0CurrentVariable", false, 0, 0));
            insert(interpreter::Variable<int> ("int", "literal", false, 0, 0));
            insert(interpreter::Array<int>("int", "0CurrentVariable", true, {0}, 0));
            insert(interpreter::Array<int> ("int", "literal", true, {0}, 0));
            insert(interpreter::Variable<float>("float", "0CurrentVariable", false, 0.0, 0));
            insert(interpreter::Variable<float> ("float", "literal", false, 0.0, 0));
            insert(interpreter::Array<float>("float", "0CurrentVariable", true, {0.0}, 0));
            insert(interpreter::Array<float> ("float", "literal", true, {0.0}, 0));
            insert(interpreter::Variable<bool>("bool", "0CurrentVariable", false, false, 0));
            insert(interpreter::Variable<bool> ("bool", "literal", false, false, 0));
            insert(interpreter::Array<bool>("float", "0CurrentVariable", true, {false}, 0));
            insert(interpreter::Array<bool> ("float", "literal", true, {false}, 0));
            insert(interpreter::Variable<std::string>("string", "0CurrentVariable", false, "", 0));
            insert(interpreter::Variable<std::string> ("string", "literal", false, "", 0));
            insert(interpreter::Array<std::string>("string", "0CurrentVariable", true, {""}, 0));
            insert(interpreter::Array<std::string> ("string", "literal", true, {""}, 0));
            insert(interpreter::Variable<char>("char", "0CurrentVariable", false, ' ', 0));
            insert(interpreter::Variable<char> ("char", "literal", false, ' ', 0));
            insert(interpreter::Array<char>("char", "0CurrentVariable", true, {' '}, 0));
            insert(interpreter::Array<char> ("char", "literal", true, {' '}, 0));
            function = false;
            array = false;
        };
        ~Interpreter() = default;

        auto find(const interpreter::Variable<int>& v);
        auto find(const interpreter::Variable<float>& v);
        auto find(const interpreter::Variable<bool>& v);
        auto find(const interpreter::Variable<std::string>& v);
        auto find(const interpreter::Variable<char>& v);
        auto find(const interpreter::Function& f);

        auto find(const interpreter::Array<int>& v);
        auto find(const interpreter::Array<float>& v);
        auto find(const interpreter::Array<bool>& v);
        auto find(const interpreter::Array<std::string>& v);
        auto find(const interpreter::Array<char>& v);

        bool insert(const interpreter::Variable<int>& v);
        bool insert(const interpreter::Variable<float>& v);
        bool insert(const interpreter::Variable<bool>& v);
        bool insert(const interpreter::Variable<std::string>& v);
        bool insert(const interpreter::Variable<char>& v);
        bool insert(const interpreter::Function& f);

        bool insert(const interpreter::Array<int>& v);
        bool insert(const interpreter::Array<float>& v);
        bool insert(const interpreter::Array<bool>& v);
        bool insert(const interpreter::Array<std::string>& v);
        bool insert(const interpreter::Array<char>& v);

        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<int>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<float>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<bool>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<std::string>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<char>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::pair<std::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>, interpreter::Function>> result);

        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Array<int>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Array<float>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Array<bool>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Array<std::string>>> result);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Array<char>>> result);

        template<typename T>
        T pop_back(const std::string& identifier);

        template<typename T>
        T get(const std::string& identifier = "0CurrentVariable");


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

#endif //TEALANG_COMPILER_CPP20_INTERPRETER_VISITOR_H
