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

    template <typename Key, typename Value>
    class Table {
    public:
        Table(){
            self = std::map<Key, Value>();
        };

        ~Table() = default;
        std::map<Key, Value> self;


        auto find(Value v);
        bool insert(Value v);
        bool found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value>> result);
        void pop_back(const std::string& identifier);
        Value get(const std::string& identifier = "0CurrentVariable");
    };

    template<typename Key, typename Value>
    auto Table<Key, Value>::find(Value v) {
        return self.find(v.identifier);
    }

    template<typename Key, typename Value>
    bool Table<Key, Value>::insert(Value v) {
        if(v.type.empty()){
            throw visitor::VariableTypeException();
        }
        auto result = find(v);
        if(found(result)){
            // Copy the result variable
            Value cpy(result -> second);
            // add the new value
            cpy.values.emplace_back(v.latestValue);
            cpy.latestValue = v.latestValue;
            // remove the result
            self.erase(result);
            // insert the copy
            insert(cpy);
            return false;
        }else{
            // The variable doesnt exist so we add a new one
            auto ret = self.insert(std::pair<Key, Value>(v.identifier, v) );
            return ret.second;
        }
    }

    template<typename Key, typename Value>
    bool Table<Key, Value>::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Value>> result) {
        return result != self.end();
    }

    template<typename Key, typename Value>
    void Table<Key, Value>::pop_back(const std::string &identifier) {
        auto result = find(Value(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        // If the vector is empty we get a sigsev
//        if(result->second.values.empty()){
////            result->second.values.emplace_back(0);
//        }
        // Copy the result variable
        Value cpy (result -> second);
        // pop the value from the copy
        cpy.values.pop_back();
        if(!cpy.values.empty()) {
            cpy.latestValue = cpy.values.back();
        }//else{
//            cpy.latestValue = nullptr;
//        }
        // remove the result=
        self.erase(result);
        // insert the copy
        insert(cpy);
    }

    template<typename Key, typename Value>
    Value Table<Key, Value>::get(const std::string &identifier) {
        auto result = find(Value(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        auto ret = result -> second;
        // pop_back case
        if(identifier == "0CurrentVariable") {
            pop_back("0CurrentVariable");
        }
        // return the popped value
        return ret;
    }
}

namespace visitor {
    class Interpreter : public Visitor {
    private:
        // Python equivalent of:
        // variableTable = {identifier: {TYPE, identifier, val, values, lineNumber}}
        interpreter::Table<std::string, interpreter::Variable<int>>                         intTable;
        interpreter::Table<std::string, interpreter::Variable<float>>                       floatTable;
        interpreter::Table<std::string, interpreter::Variable<bool>>                        boolTable;
        interpreter::Table<std::string, interpreter::Variable<std::string>>                 stringTable;
        interpreter::Table<std::string, interpreter::Variable<char>>                        charTable;
        interpreter::Table<std::string, interpreter::Variable<std::vector<int>>>            intArrayTable;
        interpreter::Table<std::string, interpreter::Variable<std::vector<float>>>          floatArrayTable;
        interpreter::Table<std::string, interpreter::Variable<std::vector<bool>>>           boolArrayTable;
        interpreter::Table<std::string, interpreter::Variable<std::vector<std::string>>>    stringArrayTable;
        interpreter::Table<std::string, interpreter::Variable<std::vector<char>>>           charArrayTable;
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
            intTable.insert(interpreter::Variable<int>("int", "0CurrentVariable", false, 0, 0));
            intTable.insert(interpreter::Variable<int> ("int", "literal", false, 0, 0));
            intArrayTable.insert(interpreter::Variable<std::vector<int>>("int", "0CurrentVariable", true, {0}, 0));
            intArrayTable.insert(interpreter::Variable<std::vector<int>>("int", "literal", true, {0}, 0));
            floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable", false, 0.0, 0));
            floatTable.insert(interpreter::Variable<float> ("float", "literal", false, 0.0, 0));
            floatArrayTable.insert(interpreter::Variable<std::vector<float>>("float", "0CurrentVariable", true, {0.0}, 0));
            floatArrayTable.insert(interpreter::Variable<std::vector<float>>("float", "literal", true, {0.0}, 0));
            boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable", false, false, 0));
            boolTable.insert(interpreter::Variable<bool> ("bool", "literal", false, false, 0));
            boolArrayTable.insert(interpreter::Variable<std::vector<bool>>("float", "0CurrentVariable", true, {false}, 0));
            boolArrayTable.insert(interpreter::Variable<std::vector<bool>> ("float", "literal", true, {false}, 0));
            stringTable.insert(interpreter::Variable<std::string>("string", "0CurrentVariable", false, "", 0));
            stringTable.insert(interpreter::Variable<std::string> ("string", "literal", false, "", 0));
            stringArrayTable.insert(interpreter::Variable<std::vector<std::string>>("string", "0CurrentVariable", true, {""}, 0));
            stringArrayTable.insert(interpreter::Variable<std::vector<std::string>> ("string", "literal", true, {""}, 0));
            charTable.insert(interpreter::Variable<char>("char", "0CurrentVariable", false, ' ', 0));
            charTable.insert(interpreter::Variable<char> ("char", "literal", false, ' ', 0));
            charArrayTable.insert(interpreter::Variable<std::vector<char>>("char", "0CurrentVariable", true, {' '}, 0));
            charArrayTable.insert(interpreter::Variable<std::vector<char>> ("char", "literal", true, {' '}, 0));
            function = false;
            array = false;
        };
        ~Interpreter() = default;
        auto find(const interpreter::Function& f);
        bool insert(const interpreter::Function& f);
        bool found(std::_Rb_tree_iterator<std::pair<const std::pair<std::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>, interpreter::Function>> result);

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
