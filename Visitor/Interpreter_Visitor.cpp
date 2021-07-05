//
// Created by aiden on 30/06/2021.
//

#include "Interpreter_Visitor.h"

namespace visitor {

    auto Interpreter::find(const interpreter::Function& f) {
        return functionTable.find( std::pair(f.identifier, f.paramTypes));
    }

    bool Interpreter::insert(const interpreter::Function& f){
        if (f.type.empty()){
            throw FunctionTypeException();
        }
        auto ret = functionTable.insert (std::pair<std::pair<std::string, std::vector<std::string>>, interpreter::Function>
                                                 (             std::make_pair(f.identifier, f.paramTypes),       f));
        return ret.second;
    }

    bool Interpreter::found(
            std::_Rb_tree_iterator<std::pair<const std::pair<std::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>, interpreter::Function>>
            result) {
        return result != functionTable.end();
    }

    void Interpreter::visit(parser::ASTProgramNode *programNode) {
        // For each statement, accept
        for(auto &statement : programNode -> statements)
            statement -> accept(this);
    }

    // Expressions
    // Literal visits add a new literal variable to the 'literalTYPE' variable in the variableTable
    void Interpreter::visit(parser::ASTLiteralNode<int> *literalNode) {
        interpreter::Variable<int> v("int", "literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        intTable.pop_back("literal");
        intTable.insert(v);
        currentType = "int";
        currentID = "literal";
        array = false;
    }

    void Interpreter::visit(parser::ASTLiteralNode<float> *literalNode) {
        interpreter::Variable<float> v("float", "literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        floatTable.pop_back("literal");
        floatTable.insert(v);
        currentType = "float";
        currentID = "literal";
        array = false;
    }

    void Interpreter::visit(parser::ASTLiteralNode<bool> *literalNode) {
        interpreter::Variable<bool> v("bool", "literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        boolTable.pop_back("literal");
        boolTable.insert(v);
        currentType = "bool";
        currentID = "literal";
        array = false;
    }

    void Interpreter::visit(parser::ASTLiteralNode<std::string> *literalNode) {
        interpreter::Variable<std::string> v("string", "literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        stringTable.pop_back("literal");
        stringTable.insert(v);
        currentType = "string";
        currentID = "literal";
    }

    void Interpreter::visit(parser::ASTLiteralNode<char> *literalNode) {
        interpreter::Variable<char> v("char", "literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        charTable.pop_back("literal");
        charTable.insert(v);
        currentType = "char";
        currentID = "literal";
        array = false;
    }

    void Interpreter::visit(parser::ASTArrayLiteralNode *arrayLiteralNode) {
        if(currentType == "int"){
            std::vector<int> arr;
            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(intTable.get(currentID).latestValue);
            }
            currentType = "int";
            intArrayTable.insert(interpreter::Variable<std::vector<int>>("int", "literal", true, arr, arrayLiteralNode->lineNumber));
        }else if(currentType == "float"){
            std::vector<float> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(floatTable.get(currentID).latestValue);
            }
            floatArrayTable.insert(interpreter::Variable<std::vector<float>>("int", "literal", true, arr, arrayLiteralNode->lineNumber));
        }else if(currentType == "bool"){
            std::vector<bool> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(boolTable.get(currentID).latestValue);
            }
            boolArrayTable.insert(interpreter::Variable<std::vector<bool>>("int", "literal", true, arr, arrayLiteralNode->lineNumber));
        }else if(currentType == "string"){
            std::vector<std::string> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(stringTable.get(currentID).latestValue);
            }
            stringArrayTable.insert(interpreter::Variable<std::vector<std::string>>("int", "literal", true, arr, arrayLiteralNode->lineNumber));
        }else if(currentType == "char"){
            std::vector<char> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(charTable.get(currentID).latestValue);
            }
            charArrayTable.insert(interpreter::Variable<std::vector<char>>("int", "literal", true, arr, arrayLiteralNode->lineNumber));
        }
        array = true;
        currentID = "literal";
    }

    void Interpreter::visit(parser::ASTBinaryNode *binaryNode) {
        // First get the operator
        std::string op = binaryNode -> op;
        // Accept left expression
        binaryNode -> left -> accept(this);
        // Push left node into 0CurrentVariable
        if(currentType == "int"){
            intTable.insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                       false,
                                                       (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
        }else if(currentType == "float"){
            floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                           false,
                                                           (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                           binaryNode->lineNumber));
        }else if(currentType == "bool"){
            boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                           false,
                                                           (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                           binaryNode->lineNumber));
        }else if(currentType == "string"){
            stringTable.insert(interpreter::Variable<std::string>("string", "0CurrentVariable",
                                                           false,
                                                           (array ? stringArrayTable.get(currentID).latestValue.at(iloc) : stringTable.get(currentID).latestValue),
                                                            binaryNode->lineNumber));
        }else if(currentType == "char"){
            charTable.insert(interpreter::Variable<char>("char", "0CurrentVariable",
                                                     false,
                                                          (array ? charArrayTable.get(currentID).latestValue.at(iloc) : charTable.get(currentID).latestValue),
                                                          binaryNode->lineNumber));
        }else{
            // should never get here
            throw std::runtime_error("Expression on line " + std::to_string(binaryNode -> lineNumber)
                                     + " has incorrect operator " + binaryNode -> op
                                     + " acting on expression of type " + currentType);
        }

        // Accept right expression
        binaryNode -> right -> accept(this);
        // We know both variables have the same type
        // So we check the currentType's type to see which operations we can do
        // check op type
        if(currentType == "int"){
            switch (lexer::determineOperatorType(binaryNode -> op)) {
                /*
                 * The following code will follow this structure
                 *
                 * insert(Variable(TYPE, CURRENTVARIABLE, EXPRESSION, LINENUMBER)
                 *
                 * the EXPRESSION is built by popping the CURRENTVARIABLE list of values and operating with the currentID variable
                 * which is retreived via the find function.
                 * here I do not check with found() because we already know this variable exists as either the
                 * literal variable or any other variable as it is changed by the visits to the literal or identifier nodes.
                 *
                 * I am sure that there is a cleaner way of doing this but I still think this is pretty clean
                 *
                 * ...albeit very long and repetitive
                 *
                 */
                case lexer::TOK_NOT_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       !=
                                                       (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       ==
                                                       (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       >
                                                       (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       <
                                                       (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       >=
                                                       (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                      <=
                                                      (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                      binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                // int and float accepted operators
                case lexer::TOK_PLUS:
                    intTable.insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       +
                                                       (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                case lexer::TOK_ASTERISK:
                    intTable.insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                      *
                                                      (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                      binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                case lexer::TOK_DIVIDE:
                    // if divide by 0 happens, gcc will raise its own error, no need to change the structure to accomodate for this
                    intTable.insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                      /
                                                      (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                      binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                case lexer::TOK_MINUS:
                    intTable.insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                      -
                                                      (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue),
                                                      binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                default:
                    // Should never get here because of the semantic pass but still included because of the default case
                    throw std::runtime_error("Expression on line " + std::to_string(binaryNode -> lineNumber)
                                             + " has incorrect operator " + binaryNode -> op
                                             + " acting between expressions of type " + currentType);
            }
        }else if(currentType == "float") {
            switch (lexer::determineOperatorType(binaryNode -> op)) {
                case lexer::TOK_NOT_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       !=
                                                       (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       ==
                                                       (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       >
                                                       (array ? (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue) : floatArrayTable.get(currentID).latestValue.at(iloc)),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       <
                                                       (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       >=
                                                       (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       <=
                                                       (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                    // int and float accepted operators
                case lexer::TOK_PLUS:
                    floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       +
                                                       (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                case lexer::TOK_ASTERISK:
                    floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                        *
                                                        (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                case lexer::TOK_DIVIDE:
                    // if divide by 0 happens, gcc will raise its own error, no need to change the structure to accomodate for this
                    floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                        /
                                                        (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                case lexer::TOK_MINUS:
                    floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                        -
                                                        (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                default:
                    // Should never get here because of the semantic pass but still included because of the default case
                    throw std::runtime_error("Expression on line " + std::to_string(binaryNode -> lineNumber)
                                             + " has incorrect operator " + binaryNode -> op
                                             + " acting between expressions of type " + currentType);
            }
        }else if(currentType == "bool"){
            switch (lexer::determineOperatorType(binaryNode -> op)) {
                case lexer::TOK_NOT_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                        !=
                                                        (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       ==
                                                       (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_AND:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       &&
                                                       (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "0CurrentVariable";
                    break;
                case lexer::TOK_OR:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       ||
                                                       (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       >
                                                       (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       <
                                                       (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       >=
                                                       (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "0CurrentVariable";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       <=
                                                       (array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                default:
                    // Should never get here because of the semantic pass but still included because of the default case
                    throw std::runtime_error("Expression on line " + std::to_string(binaryNode -> lineNumber)
                                             + " has incorrect operator " + binaryNode -> op
                                             + " acting between expressions of type " + currentType);
            }
        }else if(currentType == "string") {
            switch (lexer::determineOperatorType(binaryNode -> op)) {
                case lexer::TOK_NOT_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       stringTable.get().latestValue
                                                       !=
                                                       (array ? stringArrayTable.get(currentID).latestValue.at(iloc) : stringTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       stringTable.get().latestValue
                                                       ==
                                                       (array ? stringArrayTable.get(currentID).latestValue.at(iloc) : stringTable.get(currentID).latestValue),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_PLUS:
                    stringTable.insert(interpreter::Variable<std::string>("string", "0CurrentVariable",
                                                              false,
                                                              stringTable.get().latestValue
                                                              +
                                                              (array ? stringArrayTable.get(currentID).latestValue.at(iloc) : stringTable.get(currentID).latestValue),
                                                              binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "string";
                    break;
                default:
                    // Should never get here because of the semantic pass but still included because of the default case
                    throw std::runtime_error("Expression on line " + std::to_string(binaryNode -> lineNumber)
                                             + " has incorrect operator " + binaryNode -> op
                                             + " acting between expressions of type " + currentType);
            }
        }else if(currentType == "char") {
            switch (lexer::determineOperatorType(binaryNode->op)) {
                case lexer::TOK_NOT_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                                 false,
                                                                 charTable.get().latestValue
                                                                 !=
                                                                 (array ? charArrayTable.get(
                                                                         currentID).latestValue.at(iloc)
                                                                        : charTable.get(currentID).latestValue),
                                                                 binaryNode->lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                                 false,
                                                                 charTable.get().latestValue
                                                                 ==
                                                                 (array ? charArrayTable.get(
                                                                         currentID).latestValue.at(iloc)
                                                                        : charTable.get(currentID).latestValue),
                                                                 binaryNode->lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                default:
                    // Should never get here because of the semantic pass but still included because of the default case
                    throw std::runtime_error("Expression on line " + std::to_string(binaryNode->lineNumber)
                                             + " has incorrect operator " + binaryNode->op
                                             + " acting between expressions of type " + currentType);
            }
        }
        // Update Current ID
        currentID = "0CurrentVariable";
        // ensure array is off
        array = false;
    }

    void Interpreter::visit(parser::ASTIdentifierNode *identifierNode) {//TODO::HANDLE variable[] case
        // two cases 1 where iloc is defined (array) the other when it isnt (other types)
        if(identifierNode->ilocExprNode == nullptr){
            // Build variable shells
            array = false;
            interpreter::Variable<int> i(identifierNode -> identifier);
            interpreter::Variable<float> f(identifierNode -> identifier);
            interpreter::Variable<bool> b(identifierNode -> identifier);
            interpreter::Variable<std::string> s(identifierNode -> identifier);
            interpreter::Variable<char> c(identifierNode -> identifier);
            //struct
            // Check that a variable with this identifier exists
            auto resultI = intTable.find(i);
            if(intTable.found(resultI)) {
                // if identifier has been found
                // change current Type
                currentType = resultI -> second.type;
                // change current ID
                currentID = resultI -> second.identifier;
                // Then return
                return;
            }
            auto resultF = floatTable.find(f);
            if(floatTable.found(resultF)) {
                // if identifier has been found
                // change current Type
                currentType = resultF -> second.type;
                // change current ID
                currentID = resultF -> second.identifier;
                // Then return
                return;
            }
            auto resultB = boolTable.find(b);
            if(boolTable.found(resultB)) {
                // if identifier has been found
                // change current Type
                currentType = resultB -> second.type;
                // change current ID
                currentID = resultB -> second.identifier;
                // Then return
                return;
            }
            auto resultS = stringTable.find(s);
            if(stringTable.found(resultS)) {
                // if identifier has been found
                // change current Type
                currentType = resultS -> second.type;
                // change current ID
                currentID = resultS -> second.identifier;
                // Then return
                return;
            }
            auto resultC = charTable.find(c);
            if(charTable.found(resultC)) {
                // if identifier has been found
                // change current Type
                currentType = resultC -> second.type;
                // change current ID
                currentID = resultC -> second.identifier;
                // Then return
                return;
            }
        }
        // array cases

        if(identifierNode->ilocExprNode != nullptr){
            // get array iloc
            auto _cId = currentID;
            auto _cType = currentType;
            identifierNode->ilocExprNode->accept(this);
            if(currentType == "int"){
                iloc = intTable.get(currentID).latestValue;
            }else if(currentType == "float"){
                iloc = floatTable.get(currentID).latestValue;
            }else{
                throw std::runtime_error("Variable with identifier " + identifierNode->getID() + " called on line "
                                         + std::to_string(identifierNode->lineNumber) + " has not incorrect value between [].");
            }
            currentID = _cId;
            currentType = _cType;
        }else{
            iloc = -1;
        }
        // Build variable shells
        interpreter::Variable<std::vector<int>> i(identifierNode -> identifier);
        interpreter::Variable<std::vector<float>> f(identifierNode -> identifier);
        interpreter::Variable<std::vector<bool>> b(identifierNode -> identifier);
        interpreter::Variable<std::vector<std::string>> s(identifierNode -> identifier);
        interpreter::Variable<std::vector<char>> c(identifierNode -> identifier);
        //struct
        array = true;
        // Check that a variable with this identifier exists
        auto resultI = intArrayTable.find(i);
        if(intArrayTable.found(resultI)) {
            // if identifier has been found
            // change current Type
            currentType = resultI -> second.type;
            // change current ID
            currentID = resultI -> second.identifier;
            // Then return
            return;
        }
        auto resultF = floatArrayTable.find(f);
        if(floatArrayTable.found(resultF)) {
            // if identifier has been found
            // change current Type
            currentType = resultF -> second.type;
            // change current ID
            currentID = resultF -> second.identifier;
            // Then return
            return;
        }
        auto resultB = boolArrayTable.find(b);
        if(boolArrayTable.found(resultB)) {
            // if identifier has been found
            // change current Type
            currentType = resultB -> second.type;
            // change current ID
            currentID = resultB -> second.identifier;
            // Then return
            return;
        }
        auto resultS = stringArrayTable.find(s);
        if(stringArrayTable.found(resultS)) {
            // if identifier has been found
            // change current Type
            currentType = resultS -> second.type;
            // change current ID
            currentID = resultS -> second.identifier;
            // Then return
            return;
        }
        auto resultC = charArrayTable.find(c);
        if(charArrayTable.found(resultC)) {
            // if identifier has been found
            // change current Type
            currentType = resultC -> second.type;
            // change current ID
            currentID = resultC -> second.identifier;
            // Then return
            return;
        }
        // Variable hasn't been found (should never get here)
        throw std::runtime_error("Variable with identifier " + i.identifier + " called on line "
                                 + std::to_string(identifierNode->lineNumber) + " has not been declared.");
    }

    void Interpreter::visit(parser::ASTUnaryNode *unaryNode) {
        // visit the expression to get the type and id
        unaryNode -> exprNode -> accept(this);
        // now we check the type
        if(currentType == "int"){
            intTable.insert(interpreter::Variable<int>("int", "string", false, intTable.get(currentID).latestValue * -1, unaryNode -> lineNumber));
        }else if(currentType == "float"){
            floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable", false, floatTable.get(currentID).latestValue * -1, unaryNode -> lineNumber));
        }else if(currentType == "bool"){
            boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable", false, ! boolTable.get(currentID).latestValue, unaryNode -> lineNumber));
        }else{
            // should get here
            throw std::runtime_error("Expression on line " + std::to_string(unaryNode -> lineNumber)
                                     + " has incorrect operator " + unaryNode -> op
                                     + " acting for expression of type " + currentType);
        }
    }

    void Interpreter::visit(parser::ASTFunctionCallNode *functionCallNode) {
        // get parameters
        std::vector<std::string> paramTypes;
        for (const auto& param : functionCallNode->parameters){
            // this visit will check if the variables exist
            param->accept(this);
            // store the types (in order) so than we can confirm that the types are good
            // when the function is found
            paramTypes.emplace_back(currentType);
        }
        // Generate Function
        interpreter::Function f(functionCallNode -> identifier -> identifier, paramTypes);
        // find actual function
        auto result = find(f);
        if(!found(result)) {
            // Should never get here
            throw std::runtime_error("Function with identifier " + f.identifier + " called on line "
                                     + std::to_string(functionCallNode->lineNumber) + " has not been declared.");
        }
        f = result->second;
        // go over the function parameters
        // and make sure to update these variables according to the parameters passed
        for (int i = 0; i < functionCallNode -> parameters.size(); ++i){
            // this visit will check if the variables exist
            functionCallNode -> parameters.at(i) -> accept(this);
            // This visit updates the currentID and currentType
            // store current ID so that we dont need to visit the parameters again to pop their values
            toPop.emplace_back(interpreter::Popable(currentType, f.paramIDs.at(i), array));
            if(currentType == "int"){
                /* Update the currentID variable by emplacing back
                 * to f.paramIDs.at(i) variable
                 * what is found inside the variable with identifier currentID
                 * which we got from visiting the parameter expression
                 * this temporarily overwrites any global variable
                 * Once the block is function block is visited we pop back these variables to clear memory
                */
                array ?
                intArrayTable.insert(interpreter::Variable<std::vector<int>>("int", f.paramIDs.at(i), true, intArrayTable.get(currentID).latestValue, functionCallNode -> lineNumber))
                      :
                intTable.insert(interpreter::Variable<int>("int", f.paramIDs.at(i), false, intTable.get(currentID).latestValue, functionCallNode -> lineNumber));
            }else if(currentType == "float"){
                array ?
                floatArrayTable.insert(interpreter::Variable<std::vector<float>>("float", f.paramIDs.at(i), true, floatArrayTable.get(currentID).latestValue, functionCallNode -> lineNumber))
                      :
                floatTable.insert(interpreter::Variable<float>("float", f.paramIDs.at(i), false, floatTable.get(currentID).latestValue, functionCallNode -> lineNumber));
            }else if(currentType == "bool"){
                array ?
                boolArrayTable.insert(interpreter::Variable<std::vector<bool>>("bool", f.paramIDs.at(i), true, boolArrayTable.get(currentID).latestValue, functionCallNode -> lineNumber))
                      :
                boolTable.insert(interpreter::Variable<bool>("bool", f.paramIDs.at(i), false, boolTable.get(currentID).latestValue, functionCallNode -> lineNumber));
            }else if(currentType == "string"){
                array ?
                stringArrayTable.insert(interpreter::Variable<std::vector<std::string>>("string", f.paramIDs.at(i), true, stringArrayTable.get(currentID).latestValue, functionCallNode -> lineNumber))
                      :
                stringTable.insert(interpreter::Variable<std::string>("string", f.paramIDs.at(i), false, stringTable.get(currentID).latestValue, functionCallNode -> lineNumber));
            }else if(currentType == "char"){
                array ?
                charArrayTable.insert(interpreter::Variable<std::vector<char>>("char", f.paramIDs.at(i), true, charArrayTable.get(currentID).latestValue, functionCallNode -> lineNumber))
                      :
                charTable.insert(interpreter::Variable<char>("char", f.paramIDs.at(i), false, charTable.get(currentID).latestValue, functionCallNode -> lineNumber));
            }
        }
        // Ok so now we have updated the arguments, so we can call the actual function to run
        function = true;
        f.blockNode -> accept(this);
        function = false;
        // the function has completed its run now we pop back the variables we added
        for (const auto& pair : toPop){
            /*
             * Now we pop the variables
            */
            if(pair.type == "int"){
                pair.array ? intArrayTable.pop_back(pair.id) : intTable.pop_back(pair.id);
            }else if(pair.type == "float"){
                pair.array ? floatArrayTable.pop_back(pair.id) : floatTable.pop_back(pair.id);
            }else if(pair.type == "bool"){
                pair.array ? boolArrayTable.pop_back(pair.id) : boolTable.pop_back(pair.id);
            }else if(pair.type == "string"){
                pair.array ? stringArrayTable.pop_back(pair.id) : stringTable.pop_back(pair.id);
            }else if(pair.type == "char"){
                pair.array ? charArrayTable.pop_back(pair.id) : charTable.pop_back(pair.id);
            }
        }
        toPop = std::vector<interpreter::Popable>();
    }
    // Expressions

    // Statements

    void Interpreter::visit(parser::ASTSFunctionCallNode *sFunctionCallNode) {
        // get parameters
        std::vector<std::string> paramTypes;
        for (const auto& param : sFunctionCallNode->parameters){
            // this visit will check if the variables exist
            param->accept(this);
            // store the types (in order) so than we can confirm that the types are good
            // when the function is found
            paramTypes.emplace_back(currentType);
        }
        // Generate Function
        interpreter::Function f(sFunctionCallNode -> identifier -> identifier, paramTypes);
        // find actual function
        auto result = find(f);
        if(! found(result)) {
            // Should never get here
            throw std::runtime_error("Function with identifier " + f.identifier + " called on line "
                                     + std::to_string(sFunctionCallNode->lineNumber) + " has not been declared.");
        }
        f = result->second;
        // go over the function parameters
        // and make sure to update these variables according to the parameters passed
        for (int i = 0; i < sFunctionCallNode -> parameters.size(); ++i){
            // this visit will check if the variables exist
            sFunctionCallNode -> parameters.at(i) -> accept(this);
            // This visit updates the currentID and currentType
            // store current ID so that we dont need to visit the parameters again to pop their values
            toPop.emplace_back(interpreter::Popable(currentType, f.paramIDs.at(i), array));
            if(currentType == "int"){
                /* Update the currentID variable by emplacing back
                 * to f.paramIDs.at(i) variable
                 * what is found inside the variable with identifier currentID
                 * which we got from visiting the parameter expression
                 * this temporarily overwrites any global variable
                 * Once the block is function block is visited we pop back these variables to clear memory
                */
                array ?
                intArrayTable.insert(interpreter::Variable<std::vector<int>>("int", f.paramIDs.at(i), true, intArrayTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber))
                :
                intTable.insert(interpreter::Variable<int>("int", f.paramIDs.at(i), false, intTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
            }else if(currentType == "float"){
                array ?
                floatArrayTable.insert(interpreter::Variable<std::vector<float>>("float", f.paramIDs.at(i), true, floatArrayTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber))
                :
                floatTable.insert(interpreter::Variable<float>("float", f.paramIDs.at(i), false, floatTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
            }else if(currentType == "bool"){
                array ?
                boolArrayTable.insert(interpreter::Variable<std::vector<bool>>("bool", f.paramIDs.at(i), true, boolArrayTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber))
                :
                boolTable.insert(interpreter::Variable<bool>("bool", f.paramIDs.at(i), false, boolTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
            }else if(currentType == "string"){
                array ?
                stringArrayTable.insert(interpreter::Variable<std::vector<std::string>>("string", f.paramIDs.at(i), true, stringArrayTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber))
                :
                stringTable.insert(interpreter::Variable<std::string>("string", f.paramIDs.at(i), false, stringTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
            }else if(currentType == "char"){
                array ?
                charArrayTable.insert(interpreter::Variable<std::vector<char>>("char", f.paramIDs.at(i), true, charArrayTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber))
                :
                charTable.insert(interpreter::Variable<char>("char", f.paramIDs.at(i), false, charTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
            }
        }
        // Ok so now we have updated the arguments, so we can call the actual function to run
        function = true;
        f.blockNode -> accept(this);
        function = false;
        // the function has completed its run now we pop back the variables we added
        for (const auto& pair : toPop){
            /*
             * Now we pop the variables
            */
            if(pair.type == "int"){
                pair.array ? intArrayTable.pop_back(pair.id) : intTable.pop_back(pair.id);
            }else if(pair.type == "float"){
                pair.array ? floatArrayTable.pop_back(pair.id) : floatTable.pop_back(pair.id);
            }else if(pair.type == "bool"){
                pair.array ? boolArrayTable.pop_back(pair.id) : boolTable.pop_back(pair.id);
            }else if(pair.type == "string"){
                pair.array ? stringArrayTable.pop_back(pair.id) : stringTable.pop_back(pair.id);
            }else if(pair.type == "char"){
                pair.array ? charArrayTable.pop_back(pair.id) : charTable.pop_back(pair.id);
            }
        }
        toPop = std::vector<interpreter::Popable>();
    }

    void Interpreter::visit(parser::ASTDeclarationNode *declarationNode) {
        // We dont visit the identifier as this would produce an error as the interpreter expects
        // a variable with identifier as provided to exist
        // instead we directly assign the variable when the interpreter::Variable is created
//        declarationNode -> identifier -> accept(this);

        // Visit the expression to get the current Type and Current Id
        if(declarationNode->exprNode != nullptr){
            declarationNode->exprNode->accept(this);
        }else{
            // array declaration case
            if(declarationNode->identifier->ilocExprNode != nullptr){
                // get array size
                currentType = declarationNode -> type;
                auto _cType = currentType;
                declarationNode->identifier->ilocExprNode->accept(this);
                int size;
                if(currentType == "int"){
                    size = intTable.get(currentID).latestValue;
                }else if(currentType == "float"){
                    size = floatTable.get(currentID).latestValue;
                }else{
                    throw std::runtime_error("Variable with identifier " + declarationNode->identifier->getID() + " called on line "
                                             + std::to_string(declarationNode->lineNumber) + " has not incorrect value between [].");
                }
                currentType = _cType;
                // amend array
                if(currentType == "int"){
                    intArrayTable.insert (
                            interpreter::Variable<std::vector<int>>(currentType, declarationNode -> identifier -> identifier,
                                                                    true, std::vector<int>(size), declarationNode -> lineNumber)
                    );
                }else if(currentType == "float"){
                    floatArrayTable.insert (
                            interpreter::Variable<std::vector<float>>(currentType, declarationNode -> identifier -> identifier,
                                                         true, std::vector<float>(size), declarationNode -> lineNumber)
                    );
                }else if(currentType == "bool"){
                    boolArrayTable.insert (
                            interpreter::Variable<std::vector<bool>>(currentType, declarationNode -> identifier -> identifier,
                                                        true, std::vector<bool>(size), declarationNode -> lineNumber)
                    );
                }else if(currentType == "string"){
                    stringArrayTable.insert (
                            interpreter::Variable<std::vector<std::string>>(currentType, declarationNode -> identifier -> identifier,
                                                               true, std::vector<std::string>(size), declarationNode -> lineNumber)
                    );
                }else if(currentType == "char"){
                    charArrayTable.insert (
                            interpreter::Variable<std::vector<char>>(currentType, declarationNode -> identifier -> identifier,
                                                        true, std::vector<char>(size), declarationNode -> lineNumber)
                    );
                }
                if(function){
                    toPop.emplace_back(interpreter::Popable(currentType, declarationNode -> identifier -> identifier, true));
                }
                return;
            }else{
               // struct case
               // find struct build
               // insert new struct variable
            }
        }

        // Now we have an updated current type and id
        // Create a variable with this information

        // Insert the new variable

        if(currentType == "int"){
            array ? intArrayTable.insert (
                    interpreter::Variable<std::vector<int>>(currentType, declarationNode -> identifier -> identifier, true, intArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            intTable.insert (
                    interpreter::Variable<int>(currentType, declarationNode -> identifier -> identifier, false, intTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "float"){
            array ? floatArrayTable.insert (
                    interpreter::Variable<std::vector<float>>(currentType, declarationNode -> identifier -> identifier, true, floatArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            floatTable.insert (
                    interpreter::Variable<float>(currentType, declarationNode -> identifier -> identifier, false, floatTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "bool"){
            array ? boolArrayTable.insert (
                    interpreter::Variable<std::vector<bool>>(currentType, declarationNode -> identifier -> identifier, true, boolArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            boolTable.insert (
                    interpreter::Variable<bool>(currentType, declarationNode -> identifier -> identifier, false, boolTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "string"){
            array ? stringArrayTable.insert (
                    interpreter::Variable<std::vector<std::string>>(currentType, declarationNode -> identifier -> identifier, true, stringArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            stringTable.insert (
                    interpreter::Variable<std::string>(currentType, declarationNode -> identifier -> identifier, false, stringTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "char"){
            array ? charArrayTable.insert (
                    interpreter::Variable<std::vector<char>>(currentType, declarationNode -> identifier -> identifier, true, charArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            charTable.insert (
                    interpreter::Variable<char>(currentType, declarationNode -> identifier -> identifier, false, charTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }

        if(function){
            toPop.emplace_back(interpreter::Popable(currentType, declarationNode -> identifier -> identifier, array));
        }
        array = false;
    }

    void Interpreter::visit(parser::ASTAssignmentNode *assignmentNode) {
        // We visit the identifier node now as we expect the variable to exist
        assignmentNode -> identifier -> accept(this);
        // We can update the type and identifier local variables
        // These two variables define the found variable
        std::string type = currentType;
        std::string id = currentID;
        // the array variable will also tell us if an array is being accessed right now
        bool accessing_array = array;
        // Visit the expression to get the current Type and Current Id
        array = false;
        assignmentNode -> exprNode -> accept(this);
        bool assigning_array = array;
        // Now we have an updated current type and id
        // These two variables define what we will give id
        // Replace the variable value
        // Replacement can be done by popping then inserting

        // accessing_array assignment cases

        if(accessing_array){
//            if(assignmentNode->identifier->ilocExprNode != nullptr) {
                // get array iloc
//                assignmentNode->identifier->ilocExprNode->accept(this);
                if(currentType == "int"){
                    auto result = intArrayTable.find(interpreter::Variable<std::vector<int>>(id));
                    if(!intArrayTable.found(result)){
                        throw std::runtime_error("Failed to find variable with identifier " + id);
                    }
                    std::vector<int> cpy = (result -> second.values)[0];
                    cpy[iloc] = intTable.get(currentID).latestValue;
                    // remove the old array
                    intArrayTable.pop_back(id);
                    // insert the copy
                    intArrayTable.insert (
                            interpreter::Variable<std::vector<int>>(type, id, false, cpy, assignmentNode -> lineNumber)
                    );
                }else if(currentType == "float"){
                    auto result = floatArrayTable.find(interpreter::Variable<std::vector<float>>(id));
                    if(!floatArrayTable.found(result)){
                        throw std::runtime_error("Failed to find variable with identifier " + id);
                    }
                    std::vector<float> cpy = (result -> second.values)[0];
                    cpy[iloc] = floatTable.get(currentID).latestValue;
                    // remove the old array
                    floatArrayTable.pop_back(id);
                    // insert the copy
                    floatArrayTable.insert (
                            interpreter::Variable<std::vector<float>>(type, id, false, cpy, assignmentNode -> lineNumber)
                    );
                }else if(currentType == "bool"){
                    auto result = boolArrayTable.find(interpreter::Variable<std::vector<bool>>(id));
                    if(!boolArrayTable.found(result)){
                        throw std::runtime_error("Failed to find variable with identifier " + id);
                    }
                    std::vector<bool> cpy = (result -> second.values)[0];
                    cpy[iloc] = boolTable.get(currentID).latestValue;
                    // remove the old array
                    boolArrayTable.pop_back(id);
                    // insert the copy
                    boolArrayTable.insert (
                            interpreter::Variable<std::vector<bool>>(type, id, false, cpy, assignmentNode -> lineNumber)
                    );
                }else if(currentType == "string"){
                    auto result = stringArrayTable.find(interpreter::Variable<std::vector<std::string>>(id));
                    if(!stringArrayTable.found(result)){
                        throw std::runtime_error("Failed to find variable with identifier " + id);
                    }
                    std::vector<std::string> cpy = (result -> second.values)[0];
                    cpy[iloc] = stringTable.get(currentID).latestValue;
                    // remove the old array
                    stringArrayTable.pop_back(id);
                    // insert the copy
                    stringArrayTable.pop_back(id);
                    stringArrayTable.insert (
                            interpreter::Variable<std::vector<std::string>>(type, id, false, cpy, assignmentNode -> lineNumber)
                    );
                }else if(currentType == "char"){
                    auto result = charArrayTable.find(interpreter::Variable<std::vector<char>>(id));
                    if(!charArrayTable.found(result)){
                        throw std::runtime_error("Failed to find variable with identifier " + id);
                    }
                    std::vector<char> cpy = (result -> second.values)[0];
                    cpy[iloc] = charTable.get(currentID).latestValue;
                    // remove the old array
                    charArrayTable.pop_back(id);
                    // insert the copy
                    charArrayTable.insert (
                            interpreter::Variable<std::vector<char>>(type, id, false, cpy, assignmentNode -> lineNumber)
                    );
                }
                if(function){
                    toPop.emplace_back(interpreter::Popable(currentType, id, true));
                }
                array = false;
                return;
            }
//        }

        // assigning_array cases

        if(assigning_array){
            if(currentType == "int"){
                intArrayTable.insert (
                        interpreter::Variable<std::vector<int>>(currentType, id, true, intArrayTable.get(currentID).latestValue, assignmentNode -> lineNumber)
                );
            }else if(currentType == "float"){
                floatArrayTable.insert (
                        interpreter::Variable<std::vector<float>>(currentType, id, true, floatArrayTable.get(currentID).latestValue, assignmentNode -> lineNumber)
                );
            }else if(currentType == "bool"){
                boolArrayTable.insert (
                        interpreter::Variable<std::vector<bool>>(currentType, id, true, boolArrayTable.get(currentID).latestValue, assignmentNode -> lineNumber)
                );
            }else if(currentType == "string"){
                stringArrayTable.insert (
                        interpreter::Variable<std::vector<std::string>>(currentType, id, true, stringArrayTable.get(currentID).latestValue, assignmentNode -> lineNumber)
                );
            }else if(currentType == "char array"){
                charArrayTable.insert (
                        interpreter::Variable<std::vector<char>>(currentType, id, true, charArrayTable.get(currentID).latestValue, assignmentNode -> lineNumber)
                );
            }
            if(function){
                toPop.emplace_back(interpreter::Popable(currentType, id, true));
            }
            array = false;
            return;
        }

        // Normal variable cases
        if(currentType == "int"){
            intTable.pop_back(id);
            intTable.insert (
                    interpreter::Variable<int>(type, id, false, intTable.get(currentID).latestValue, assignmentNode -> lineNumber)
            );
        }else if(currentType == "float"){
            floatTable.pop_back(id);
            floatTable.insert (
                    interpreter::Variable<float>(type, id, false, floatTable.get(currentID).latestValue, assignmentNode -> lineNumber)
            );
        }else if(currentType == "bool"){
            boolTable.pop_back(id);
            boolTable.insert (
                    interpreter::Variable<bool>(type, id, false, boolTable.get(currentID).latestValue, assignmentNode -> lineNumber)
            );
        }else if(currentType == "string"){
            stringTable.pop_back(id);
            stringTable.insert (
                    interpreter::Variable<std::string>(type, id, false, stringTable.get(currentID).latestValue, assignmentNode -> lineNumber)
            );
        }else if(currentType == "char"){
            charTable.pop_back(id);
            charTable.insert (
                    interpreter::Variable<char>(type, id, false, charTable.get(currentID).latestValue, assignmentNode -> lineNumber)
            );
        }
        if(function){
            toPop.emplace_back(interpreter::Popable(currentType, assignmentNode -> identifier -> identifier, false));
        }
    }

    void Interpreter::visit(parser::ASTPrintNode *printNode) {
        // Visit expression node to get current type
        printNode -> exprNode -> accept(this);
        if(currentType == "int"){
            std::cout << (array ? intArrayTable.get(currentID).latestValue.at(iloc) : intTable.get(currentID).latestValue) << std::endl;
        }else if(currentType == "float"){
            std::cout << (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue) << std::endl;
        }else if(currentType == "bool"){
            std::cout << ((array ? boolArrayTable.get(currentID).latestValue.at(iloc) : boolTable.get(currentID).latestValue) ? "true" : "false") << std::endl;
        }else if(currentType == "string"){
            std::cout << (array ? stringArrayTable.get(currentID).latestValue.at(iloc) : stringTable.get(currentID).latestValue) << std::endl;
        }else if(currentType == "char"){
            std::cout << (array ? charArrayTable.get(currentID).latestValue.at(iloc) : charTable.get(currentID).latestValue) << std::endl;
        }
        array = false;
    }

    void Interpreter::visit(parser::ASTBlockNode *blockNode) {
        // Visit each statement in the block
        for(auto &statement : blockNode -> statements)
            statement -> accept(this);
    }

    void Interpreter::visit(parser::ASTIfNode *ifNode) {
        // Get the condition
        ifNode -> condition -> accept(this);
        // follow the if structure
        if(boolTable.get(currentID).latestValue){
            // do the if block
            ifNode -> ifBlock -> accept(this);
        }else{
            // If there is an else block, do it too
            if(ifNode -> elseBlock != nullptr)
                ifNode -> elseBlock -> accept(this);
        }
    }

    void Interpreter::visit(parser::ASTForNode *forNode) {
        // Get the declaration
        if(forNode -> declaration != nullptr)
            forNode -> declaration -> accept(this);
        // Get the condition
        forNode -> condition -> accept(this);

        while(boolTable.get(currentID).latestValue){
            // do the loop block
            forNode -> loopBlock -> accept(this);

            // Now go over the assignment
            if(forNode -> assignment != nullptr)
                forNode -> assignment -> accept(this);
            // Get the condition again
            forNode -> condition -> accept(this);
        }
    }

    void Interpreter::visit(parser::ASTWhileNode *whileNode) {
        // Get the condition
        whileNode -> condition -> accept(this);

        while(boolTable.get(currentID).latestValue){
            // do the loop block
            whileNode -> loopBlock -> accept(this);

            // Get the condition again
            whileNode -> condition -> accept(this);
        }
    }

    void Interpreter::visit(parser::ASTFunctionDeclarationNode *functionDeclarationNode) {
        // We dont visit the identifier as this would produce an error as the interpreter expects
        // a variable with identifier as provided to exist
        // instead we directly assign the variable when the interpreter::Variable is created
        // functionDeclarationNode -> identifier -> accept(this);
        // Split the params into two vectors

        std::vector<std::string> paramTypes;
        std::vector<std::string> paramIDs;
        for (auto & parameter : functionDeclarationNode->parameters){
            paramTypes.emplace_back(parameter.second);
            paramIDs.emplace_back(parameter.first);
        }

        // Insert the new function
        insert (
                interpreter::Function(functionDeclarationNode->type,
                                      functionDeclarationNode -> identifier -> identifier,
                                      paramTypes, paramIDs,functionDeclarationNode->functionBlock,
                                      functionDeclarationNode -> lineNumber)
        );
    }

    void Interpreter::visit(parser::ASTReturnNode *returnNode) {
        // Update current expression
        returnNode -> exprNode -> accept(this);
        // make sure to not pop_back the returns value
        auto save = interpreter::Popable(currentType, currentID, array);
        // find the return
        for (auto it = toPop.begin(); it != toPop.end(); ++it){
            if(it->id == save.id){
                toPop.erase(it);
                if(it == toPop.end()) break;
            }
        }

    }

    void Interpreter::visit(parser::ASTStructNode *structNode) {
        std::cout << "helloo" << std::endl;
    }
    // Statements
}