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
            throw semantic::FunctionTypeException();
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
        interpreter::Variable<int> v("int", "0literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        intTable.pop_back("0literal");
        intTable.insert(v);
        currentType = "int";
        currentID = "0literal";
        array = false;
    }

    void Interpreter::visit(parser::ASTLiteralNode<float> *literalNode) {
        interpreter::Variable<float> v("float", "0literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        floatTable.pop_back("0literal");
        floatTable.insert(v);
        currentType = "float";
        currentID = "0literal";
        array = false;
    }

    void Interpreter::visit(parser::ASTLiteralNode<bool> *literalNode) {
        interpreter::Variable<bool> v("bool", "0literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        boolTable.pop_back("0literal");
        boolTable.insert(v);
        currentType = "bool";
        currentID = "0literal";
        array = false;
    }

    void Interpreter::visit(parser::ASTLiteralNode<std::string> *literalNode) {
        interpreter::Variable<std::string> v("string", "0literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        stringTable.pop_back("0literal");
        stringTable.insert(v);
        currentType = "string";
        currentID = "0literal";
    }

    void Interpreter::visit(parser::ASTLiteralNode<char> *literalNode) {
        interpreter::Variable<char> v("char", "0literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        charTable.pop_back("0literal");
        charTable.insert(v);
        currentType = "char";
        currentID = "0literal";
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
            intArrayTable.insert(interpreter::Variable<std::vector<int>>("int", "0literal", true, arr, arrayLiteralNode->lineNumber));
        }else if(currentType == "float"){
            std::vector<float> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(floatTable.get(currentID).latestValue);
            }
            floatArrayTable.insert(interpreter::Variable<std::vector<float>>("int", "0literal", true, arr, arrayLiteralNode->lineNumber));
        }else if(currentType == "bool"){
            std::vector<bool> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(boolTable.get(currentID).latestValue);
            }
            boolArrayTable.insert(interpreter::Variable<std::vector<bool>>("int", "0literal", true, arr, arrayLiteralNode->lineNumber));
        }else if(currentType == "string"){
            std::vector<std::string> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(stringTable.get(currentID).latestValue);
            }
            stringArrayTable.insert(interpreter::Variable<std::vector<std::string>>("int", "0literal", true, arr, arrayLiteralNode->lineNumber));
        }else if(currentType == "char"){
            std::vector<char> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(charTable.get(currentID).latestValue);
            }
            charArrayTable.insert(interpreter::Variable<std::vector<char>>("int", "0literal", true, arr, arrayLiteralNode->lineNumber));
        }
        array = true;
        currentID = "0literal";
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
        std::string _struct_copy = structID;
        // clear for now
        structID = "";
        binaryNode -> right -> accept(this);
        structID = _struct_copy;
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
                                                       (array ? floatArrayTable.get(currentID).latestValue.at(iloc) : floatTable.get(currentID).latestValue),
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

    void Interpreter::visit(parser::ASTIdentifierNode *identifierNode) {
        // two cases 1 where iloc is defined (array) the other when it isnt (other types)
        std::string id = "";
        if(!structID.empty()){
            id += structID + ".";
        }
        id += identifierNode -> getID();
        if(identifierNode->ilocExprNode == nullptr){
            // Build variable shells
            array = false;
            interpreter::Variable<int> i(id);
            interpreter::Variable<float> f(id);
            interpreter::Variable<bool> b(id);
            interpreter::Variable<std::string> s(id);
            interpreter::Variable<char> c(id);
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
        interpreter::Variable<std::vector<int>> i(id);
        interpreter::Variable<std::vector<float>> f(id);
        interpreter::Variable<std::vector<bool>> b(id);
        interpreter::Variable<std::vector<std::string>> s(id);
        interpreter::Variable<std::vector<char>> c(id);
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
        // iof not found than it is a struct
        currentType = struct_variable.find((structID.empty() ? identifierNode->getID() : structID))->second;
        currentID = identifierNode->getID();
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
        interpreter::Function f(functionCallNode -> identifier -> getID(), paramTypes);
        // find actual function
        auto result = find(f);
        if(! found(result)) {
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
            if(lexer::isStruct(currentType)){
//                parser::ASTDeclarationNode declarationNode(currentType,
//                                                           std::make_shared<parser::ASTIdentifierNode>(f.paramIDs.at(i), nullptr, nullptr, functionCallNode->lineNumber),
//                                                           nullptr, functionCallNode->lineNumber);
//                declarationNode.accept(this);

            }else
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

        if(!functionCallNode->identifier->isEmpty()){
            auto parent = parser::ASTIdentifierNode(functionCallNode->identifier->identifier, functionCallNode->identifier->getChild(), functionCallNode->identifier->ilocExprNode, functionCallNode->identifier->lineNumber);
            auto child = parent.getChild();
            structID = "";
            while(child != nullptr){
                structID += functionCallNode->identifier->identifier;
                parent = parser::ASTIdentifierNode(child);
                child = child->getChild();
            }
        }

        f.blockNode -> accept(this);
        function = false;
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
        interpreter::Function f(sFunctionCallNode -> identifier -> getID(), paramTypes);
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
            if(lexer::isStruct(currentType)){
//                parser::ASTDeclarationNode declarationNode(currentType,
//                                                           std::make_shared<parser::ASTIdentifierNode>(f.paramIDs.at(i), nullptr, nullptr, sFunctionCallNode->lineNumber),
//                                                           nullptr, sFunctionCallNode->lineNumber);
//                declarationNode.accept(this);

            }else
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

        if(!sFunctionCallNode->identifier->isEmpty()){
            auto parent = parser::ASTIdentifierNode(sFunctionCallNode->identifier->identifier, sFunctionCallNode->identifier->getChild(), sFunctionCallNode->identifier->ilocExprNode, sFunctionCallNode->identifier->lineNumber);
            auto child = parent.getChild();
            structID = "";
            while(child != nullptr){
                structID += sFunctionCallNode->identifier->identifier;
                parent = parser::ASTIdentifierNode(child);
                child = child->getChild();
            }
        }

        f.blockNode -> accept(this);
        function = false;
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
                            interpreter::Variable<std::vector<int>>(currentType, structID + declarationNode -> identifier -> getID(),
                                                                    true, std::vector<int>(size), declarationNode -> lineNumber)
                    );
                }else if(currentType == "float"){
                    floatArrayTable.insert (
                            interpreter::Variable<std::vector<float>>(currentType, structID + declarationNode -> identifier -> getID(),
                                                         true, std::vector<float>(size), declarationNode -> lineNumber)
                    );
                }else if(currentType == "bool"){
                    boolArrayTable.insert (
                            interpreter::Variable<std::vector<bool>>(currentType, structID + declarationNode -> identifier -> getID(),
                                                        true, std::vector<bool>(size), declarationNode -> lineNumber)
                    );
                }else if(currentType == "string"){
                    stringArrayTable.insert (
                            interpreter::Variable<std::vector<std::string>>(currentType, structID + declarationNode -> identifier -> getID(),
                                                               true, std::vector<std::string>(size), declarationNode -> lineNumber)
                    );
                }else if(currentType == "char"){
                    charArrayTable.insert (
                            interpreter::Variable<std::vector<char>>(currentType, structID + declarationNode -> identifier -> getID(),
                                                        true, std::vector<char>(size), declarationNode -> lineNumber)
                    );
                }
                if(function){
                    toPop.emplace_back(interpreter::Popable(currentType, structID + declarationNode -> identifier -> getID(), true));
                }
                return;
            }else{
                // struct case
                // find struct build
                // insert new struct variable
                // ignore self
                if(declarationNode -> identifier -> getID() != "self") {
                    structID = declarationNode->identifier->getID() + ".";
                    listOfStructs.emplace_back(declarationNode->identifier->getID());
                    struct_variable.insert(std::make_pair(declarationNode->identifier->getID(), declarationNode->type));
                    structTable.find(declarationNode->type)->second.structNode->accept(this);
                    structID = "";
                }
                return;
            }
        }

        // Now we have an updated current type and id
        // Create a variable with this information

        // Insert the new variable

        if(currentType == "int"){
            array ? intArrayTable.insert (
                    interpreter::Variable<std::vector<int>>(currentType, structID + declarationNode -> identifier -> getID(), true, intArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            intTable.insert (
                    interpreter::Variable<int>(currentType, structID + declarationNode -> identifier -> getID(), false, intTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "float"){
            array ? floatArrayTable.insert (
                    interpreter::Variable<std::vector<float>>(currentType, structID + declarationNode -> identifier -> getID(), true, floatArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            floatTable.insert (
                    interpreter::Variable<float>(currentType, structID + declarationNode -> identifier -> getID(), false, floatTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "bool"){
            array ? boolArrayTable.insert (
                    interpreter::Variable<std::vector<bool>>(currentType, structID + declarationNode -> identifier -> getID(), true, boolArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            boolTable.insert (
                    interpreter::Variable<bool>(currentType, structID + declarationNode -> identifier -> getID(), false, boolTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "string"){
            array ? stringArrayTable.insert (
                    interpreter::Variable<std::vector<std::string>>(currentType, structID + declarationNode -> identifier -> getID(), true, stringArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            stringTable.insert (
                    interpreter::Variable<std::string>(currentType, structID + declarationNode -> identifier -> getID(), false, stringTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "char"){
            array ? charArrayTable.insert (
                    interpreter::Variable<std::vector<char>>(currentType, structID + declarationNode -> identifier -> getID(), true, charArrayTable.get(currentID).latestValue, declarationNode -> lineNumber)
            )
            :
            charTable.insert (
                    interpreter::Variable<char>(currentType, structID + declarationNode -> identifier -> getID(), false, charTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }

        if(function){
            toPop.emplace_back(interpreter::Popable(currentType, structID + declarationNode -> identifier -> getID(), array));
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
            toPop.emplace_back(interpreter::Popable(currentType, id, false));
        }
    }

    void Interpreter::visit(parser::ASTPrintNode *printNode) {
        // Visit expression node to get current type
        structID = "";
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
                                      structID + functionDeclarationNode -> identifier -> getID(),
                                      paramTypes, paramIDs,functionDeclarationNode->functionBlock,
                                      functionDeclarationNode -> lineNumber)
        );
    }

    void Interpreter::visit(parser::ASTReturnNode *returnNode) {
        // Update current expression
        returnNode -> exprNode -> accept(this);
        // make sure to not pop_back the returns value
        auto save = interpreter::Popable(currentType, currentID, array);
        // Save the return
        std::vector<interpreter::Popable> struct_saves;
        if(save.type == "int"){
            if(!save.array){
                auto returnVariable = intTable.get(save.id);
                pop();
                intTable.pop_back(save.id);
                intTable.insert(returnVariable);
            }else{
                auto returnVariable = intArrayTable.get(save.id);
                pop();
                intArrayTable.pop_back(save.id);
                intArrayTable.insert(returnVariable);
            }
        }else if(save.type == "float"){
            if(!save.array) {
                auto returnVariable = floatTable.get(save.id);
                pop();
                floatTable.pop_back(save.id);
                floatTable.insert(returnVariable);
            }else{
                auto returnVariable = floatArrayTable.get(save.id);
                pop();
                floatArrayTable.pop_back(save.id);
                floatArrayTable.insert(returnVariable);
            }
        }else if(save.type == "bool"){
            if(!save.array){
                auto returnVariable = boolTable.get(save.id);
                pop();
                boolTable.pop_back(save.id);
                boolTable.insert(returnVariable);
            }else{
                auto returnVariable = boolArrayTable.get(save.id);
                pop();
                boolArrayTable.pop_back(save.id);
                boolArrayTable.insert(returnVariable);
            }
        }else if(save.type == "string"){
            if(!save.array){
                auto returnVariable = stringTable.get(save.id);
                pop();
                stringTable.pop_back(save.id);
                stringTable.insert(returnVariable);
            }else{
                auto returnVariable = stringArrayTable.get(save.id);
                pop();
                stringArrayTable.pop_back(save.id);
                stringArrayTable.insert(returnVariable);
            }
        }else if(save.type == "char"){
            if(!save.array){
                auto returnVariable = charTable.get(save.id);
                pop();
                charTable.pop_back(save.id);
                charTable.insert(returnVariable);
            }else{
                auto returnVariable = charArrayTable.get(save.id);
                pop();
                charArrayTable.pop_back(save.id);
                charArrayTable.insert(returnVariable);
            }
        }else if(lexer::isStruct(save.type)){
            // match begin of toPop with save.id + "."
            bool ok_so_far = false;
            bool _struct = false;
            bool fullstop = false;
            bool unique = true;
            if(structID.empty() || structID == "self") structID = currentID;
            // get struct variables
            for (auto & it : toPop) {
                for (int i = 0; i < it.id.size(); ++i) {
                    if (!_struct) {
                        ok_so_far = (structID.at(i) == it.id.at(i));
                        _struct = (i == structID.size() - 1);
                    }
                    fullstop = '.' == it.id.at(i);
                    if (fullstop) break;
                }
                if (ok_so_far && _struct && fullstop) { // found
                    // check if has already been placed in struct_saves
                    for(auto & struct_save : struct_saves){
                        if(struct_save.id == it.id){
                            unique = false;
                            break;
                        }
                    }
                    if(unique){
                        struct_saves.emplace_back(it);
                    }
                    unique = true;
                }
                _struct = false;
            }
            // now re insert the saved states
            for(auto & struct_save : struct_saves){
                if(struct_save.type == "int"){
                    if(!struct_save.array){
                        auto returnVariable = intTable.get(struct_save.id);
                        pop();
                        intTable.pop_back(struct_save.id);
                        intTable.insert(returnVariable);
                    }else{
                        auto returnVariable = intArrayTable.get(struct_save.id);
                        pop();
                        intArrayTable.pop_back(struct_save.id);
                        intArrayTable.insert(returnVariable);
                    }
                }else if(struct_save.type == "float"){
                    if(!struct_save.array) {
                        auto returnVariable = floatTable.get(struct_save.id);
                        pop();
                        floatTable.pop_back(struct_save.id);
                        floatTable.insert(returnVariable);
                    }else{
                        auto returnVariable = floatArrayTable.get(struct_save.id);
                        pop();
                        floatArrayTable.pop_back(struct_save.id);
                        floatArrayTable.insert(returnVariable);
                    }
                }else if(struct_save.type == "bool"){
                    if(!struct_save.array){
                        auto returnVariable = boolTable.get(struct_save.id);
                        pop();
                        boolTable.pop_back(struct_save.id);
                        boolTable.insert(returnVariable);
                    }else{
                        auto returnVariable = boolArrayTable.get(struct_save.id);
                        pop();
                        boolArrayTable.pop_back(struct_save.id);
                        boolArrayTable.insert(returnVariable);
                    }
                }
            }
        }
        pop();
    }

    void Interpreter::visit(parser::ASTStructNode *structNode) {
        // visit the block to define the functions
        structTable.insert(std::pair<std::string, interpreter::Struct>
                (std::make_pair(structNode->identifier->getID(), interpreter::Struct(structNode->identifier->getID(), structNode->structBlock))));
    }

    void Interpreter::pop() {
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
    // Statements
}