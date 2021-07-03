//
// Created by aiden on 30/06/2021.
//

#include "Interpreter_Visitor.h"

namespace visitor {
    // Tools
    bool Interpreter::insert(const interpreter::Variable<int>& v){
        if (v.type.empty()){
            throw VariableTypeException();
        }
        auto result = find(v);
        if (found(result)){
            // found the variable already
            // add the value
            // since we know that the result exists, then we can use the [] without any 'side effect' of creating a new entry
            // unfortunately intTable[identifier] kills the cpp compiler
            // so in order to pop_back a value from the values vector we have to completely replace the object
            // Copy the result variable
            interpreter::Variable<int> cpy(result -> second);
            // pop the value from the copy
            cpy.values.emplace_back(v.latestValue);
            cpy.latestValue = v.latestValue;
            // remove the result
            intTable.erase(result);
            // insert the copy
            insert(cpy);
            return false;
        }else{
            // The variable doesnt exist so we add a new one
            auto ret = intTable.insert(std::pair<std::string, interpreter::Variable<int>>(v.identifier, v) );
            return ret.second;
        }
    }

    bool Interpreter::insert(const interpreter::Variable<float>& v){
        if (v.type.empty()){
            throw VariableTypeException();
        }
        auto result = find(v);
        if (found(result)){
            // found the variable already
            // add the value
            // since we know that the result exists, then we can use the [] without any 'side effect' of creating a new entry
            // unfortunately intTable[identifier] kills the cpp compiler
            // so in order to pop_back a value from the values vector we have to completely replace the object
            // Copy the result variable
            interpreter::Variable<float> cpy(result -> second);
            // add the new value
            cpy.values.emplace_back(v.latestValue);
            cpy.latestValue = v.latestValue;
            // remove the result
            floatTable.erase(result);
            // insert the copy
            insert(cpy);
            return false;
        }else{
            // The variable doesnt exist so we add a new one
            auto ret = floatTable.insert(std::pair<std::string, interpreter::Variable<float>>(v.identifier, v) );
            return ret.second;
        }
    }

    bool Interpreter::insert(const interpreter::Variable<bool>& v){
        if (v.type.empty()){
            throw VariableTypeException();
        }
        auto result = find(v);
        if (found(result)){
            // found the variable already
            // add the value
            // since we know that the result exists, then we can use the [] without any 'side effect' of creating a new entry
            // unfortunately intTable[identifier] kills the cpp compiler
            // so in order to pop_back a value from the values vector we have to completely replace the object
            // Copy the result variable
            interpreter::Variable<bool> cpy(result -> second);
            // add the new value
            cpy.values.emplace_back(v.latestValue);
            cpy.latestValue = v.latestValue;
            // remove the result
            boolTable.erase(result);
            // insert the copy
            insert(cpy);
            return false;
        }else{
            // The variable doesnt exist so we add a new one
            auto ret = boolTable.insert(std::pair<std::string, interpreter::Variable<bool>>(v.identifier, v) );
            return ret.second;
        }
    }

    bool Interpreter::insert(const interpreter::Variable<std::string>& v){
        if (v.type.empty()){
            throw VariableTypeException();
        }
        auto result = find(v);
        if (found(result)){
            // found the variable already
            // add the value
            // since we know that the result exists, then we can use the [] without any 'side effect' of creating a new entry
            // unfortunately intTable[identifier] kills the cpp compiler
            // so in order to pop_back a value from the values vector we have to completely replace the object
            // Copy the result variable
            interpreter::Variable<std::string> cpy(result -> second);
            // add the new value
            cpy.values.emplace_back(v.latestValue);
            cpy.latestValue = v.latestValue;
            // remove the result
            stringTable.erase(result);
            // insert the copy
            insert(cpy);
            return false;
        }else{
            // The variable doesnt exist so we add a new one
            auto ret = stringTable.insert(std::pair<std::string, interpreter::Variable<std::string>>(v.identifier, v) );
            return ret.second;
        }
    }

    bool Interpreter::insert(const interpreter::Function& f){
        if (f.type.empty()){
            throw FunctionTypeException();
        }
        auto ret = functionTable.insert(std::pair<std::string, interpreter::Function>(f.identifier, f) );
        return ret.second;
    }

    std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<int>>>
    Interpreter::find(const interpreter::Variable<int>& v) {
        return intTable.find(v.identifier);
    }
    std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<float>>>
    Interpreter::find(const interpreter::Variable<float>& v) {
        return floatTable.find(v.identifier);
    }
    std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<bool>>>
    Interpreter::find(const interpreter::Variable<bool>& v) {
        return boolTable.find(v.identifier);
    }
    std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<std::string>>>
    Interpreter::find(const interpreter::Variable<std::string>& v) {
        return stringTable.find(v.identifier);
    }

    std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Function>>
    Interpreter::find(const interpreter::Function& f) {
        return functionTable.find(f.identifier);
    }

    bool Interpreter::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<int>>> result) {
        return result != intTable.end();
    }
    bool Interpreter::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<float>>> result) {
        return result != floatTable.end();
    }
    bool Interpreter::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<bool>>> result) {
        return result != boolTable.end();
    }
    bool Interpreter::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Variable<std::string>>> result) {
        return result != stringTable.end();
    }

    bool Interpreter::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, interpreter::Function>> result) {
        return result != functionTable.end();
    }

    template<> int Interpreter::pop_back<int>(const std::string &identifier) {
        auto result = find(interpreter::Variable<int>(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        // unfortunately intTable[identifier] kills the cpp compiler
        // so in order to pop_back a value from the values vector we have to completely replace the object
        // Check if the result has no values variable
        // If the vector is empty we get a sigsev
        if(result->second.values.empty()){
            result->second.values.emplace_back(0);
        }
        // Copy the result variable
        interpreter::Variable<int> cpy(result -> second);
        // pop the value from the copy
        cpy.values.pop_back();
        if(!cpy.values.empty()) {
            cpy.latestValue = cpy.values.back();
        }else{
            cpy.latestValue = 0;
        }
        // remove the result=
        intTable.erase(result);
        // insert the copy
        insert(cpy);
        // make sure to also remove variable from block variables
        return 1;
    }

    template<> float Interpreter::pop_back<float>(const std::string &identifier) {
        auto result = find(interpreter::Variable<float>(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        // unfortunately floatTable[identifier] kills the cpp compiler
        // so in order to pop_back a value from the values vector we have to completely replace the object
        // Check if the result has no values variable
        // If the vector is empty we get a sigsev
        if(result->second.values.empty()){
            result->second.values.emplace_back(0);
        }
        // Copy the result variable
        interpreter::Variable<float> cpy (result -> second);
        // pop the value from the copy
        cpy.values.pop_back();
        if(!cpy.values.empty()) {
            cpy.latestValue = cpy.values.back();
        }else{
            cpy.latestValue = 0;
        }
        // remove the result=
        floatTable.erase(result);
        // insert the copy
        insert(cpy);
        return 1;
    }

    template<> bool Interpreter::pop_back<bool>(const std::string &identifier) {
        auto result = find(interpreter::Variable<bool>(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        // unfortunately boolTable[identifier] kills the cpp compiler
        // so in order to pop_back a value from the values vector we have to completely replace the object
        // Check if the result has no values variable
        // If the vector is empty we get a sigsev
        // Copy the result variable
        if(result->second.values.empty()){
            result->second.values.emplace_back(0);
        }
        interpreter::Variable<bool> cpy(result -> second);
        // pop the value from the copy
        cpy.values.pop_back();
        if(!cpy.values.empty()) {
            cpy.latestValue = cpy.values.back();
        }else{
            cpy.latestValue = false;
        }
        // remove the result=
        boolTable.erase(result);
        // insert the copy
        insert(cpy);
        return true;
    }

    template<> std::string Interpreter::pop_back<std::string>(const std::string &identifier) {
        auto result = find(interpreter::Variable<std::string>(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        // unfortunately stringTable[identifier] kills the cpp compiler
        // so in order to pop_back a value from the values vector we have to completely replace the object
        // Check if the result has no values variable
        // If the vector is empty we get a sigsev
        if(result->second.values.empty()){
            result->second.values.emplace_back("0");
        }
        // Copy the result variable
        interpreter::Variable<std::string> cpy(result -> second);
        // pop the value from the copy
        cpy.values.pop_back();
        if(!cpy.values.empty()) {
            cpy.latestValue = cpy.values.back();
        }else{
            cpy.latestValue = "";
        }
        // remove the result
        stringTable.erase(result);
        // insert the copy
        insert(cpy);
        return "1";
    }

    template<> int Interpreter::get<int>(const std::string& identifier) {
        auto result = find(interpreter::Variable<int>(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        int ret = result -> second.latestValue;
        // pop_back case
        if (identifier == "0CurrentVariable") {
            pop_back<int>("0CurrentVariable");
        }
        // return the popped value
        return ret;
    }

    template<> float Interpreter::get<float>(const std::string& identifier) {
        auto result = find(interpreter::Variable<float>(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        float ret = result -> second.latestValue;
        // pop_back case
        if (identifier == "0CurrentVariable") {
            pop_back<float>("0CurrentVariable");
        }
        // return the popped value
        return ret;
    }

    template<> bool Interpreter::get<bool>(const std::string& identifier) {
        auto result = find(interpreter::Variable<bool>(identifier));
        if(!found(result)){
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        bool ret = result -> second.latestValue;
        // pop_back case
        if (identifier == "0CurrentVariable") {
            pop_back<bool>("0CurrentVariable");
        }
        // return the popped value
        return ret;
    }

    template<> std::string Interpreter::get<std::string>(const std::string& identifier) {
        auto result = find(interpreter::Variable<std::string>(identifier));
        if (!found(result)) {
            throw std::runtime_error("Failed to find variable with identifier " + identifier);
        }
        std::string ret = result->second.latestValue;
        // pop_back case
        if (identifier == "0CurrentVariable"){
            pop_back<std::string>("0CurrentVariable");
        }
        // return the popped value
        return ret;
    }

    // Tools

    void Interpreter::visit(parser::ASTProgramNode *programNode) {
        // For each statement, accept
        for(auto &statement : programNode -> statements)
            statement -> accept(this);
    }

    // Expressions
    // Literal visits add a new literal variable to the 'literalTYPE' variable in the variableTable
    void Interpreter::visit(parser::ASTLiteralNode<int> *literalNode) {
        interpreter::Variable<int> v("int", "literal", literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        pop_back<int>("literal");
        insert(v);
        currentType = "int";
        currentID = "literal";
    }

    void Interpreter::visit(parser::ASTLiteralNode<float> *literalNode) {
        interpreter::Variable<float> v("float", "literal", literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        pop_back<float>("literal");
        insert(v);
        currentType = "float";
        currentID = "literal";
    }

    void Interpreter::visit(parser::ASTLiteralNode<bool> *literalNode) {
        interpreter::Variable<bool> v("bool", "literal", literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        pop_back<bool>("literal");
        insert(v);
        currentType = "bool";
        currentID = "literal";
    }

    void Interpreter::visit(parser::ASTLiteralNode<std::string> *literalNode) {
        interpreter::Variable<std::string> v("string", "literal", literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        pop_back<std::string>("literal");
        insert(v);
        currentType = "string";
        currentID = "literal";
    }

    void Interpreter::visit(parser::ASTBinaryNode *binaryNode) {
        // First get the operator
        std::string op = binaryNode -> op;
        // Accept left expression
        binaryNode -> left -> accept(this);
        // Push left node into 0CurrentVariable
        if (currentType == "int"){
            insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                      get<int>(currentID),
                                                      binaryNode -> lineNumber));
        }else if (currentType == "float"){
            insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                get<float>(currentID),
                                                      binaryNode -> lineNumber));
        }else if (currentType == "bool"){
            insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                               get<bool>(currentID),
                                                      binaryNode -> lineNumber));
        }else if (currentType == "string"){
            insert(interpreter::Variable<std::string>("string", "0CurrentVariable",
                                                      get<std::string>(currentID),
                                                      binaryNode -> lineNumber));
        }

        // Accept right expression
        binaryNode -> right -> accept(this);
        // We know both variables have the same type
        // So we check the currentType's type to see which operations we can do
        // check op type
        if (currentType == "int"){
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
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<int>()
                                                       !=
                                                       get<int>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<int>()
                                                       ==
                                                       get<int>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<int>()
                                                       >
                                                       get<int>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<int>()
                                                       <
                                                       get<int>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<int>()
                                                       >=
                                                       get<int>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                      get<int>()
                                                      <=
                                                      get<int>(currentID),
                                                      binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                // int and float accepted operators
                case lexer::TOK_PLUS:
                    insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                       get<int>()
                                                       +
                                                       get<int>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                case lexer::TOK_ASTERISK:
                    insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                      get<int>()
                                                      *
                                                      get<int>(currentID),
                                                      binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                case lexer::TOK_DIVIDE:
                    // if divide by 0 happens, gcc will raise its own error, no need to change the structure to accomodate for this
                    insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                      get<int>()
                                                      /
                                                      get<int>(currentID),
                                                      binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                case lexer::TOK_MINUS:
                    insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                      get<int>()
                                                      -
                                                      get<int>(currentID),
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
        }else if (currentType == "float") {
            switch (lexer::determineOperatorType(binaryNode -> op)) {
                case lexer::TOK_NOT_EQAUL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<float>()
                                                       !=
                                                       get<float>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<float>()
                                                       ==
                                                       get<float>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<float>()
                                                       >
                                                       get<float>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<float>()
                                                       <
                                                       get<float>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<float>()
                                                       >=
                                                       get<float>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<float>()
                                                       <=
                                                       get<float>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                    // int and float accepted operators
                case lexer::TOK_PLUS:
                    insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                       get<float>()
                                                       +
                                                       get<float>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                case lexer::TOK_ASTERISK:
                    insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                        get<float>()
                                                        *
                                                        get<float>(currentID),
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                case lexer::TOK_DIVIDE:
                    // if divide by 0 happens, gcc will raise its own error, no need to change the structure to accomodate for this
                    insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                        get<float>()
                                                        /
                                                        get<float>(currentID),
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                case lexer::TOK_MINUS:
                    insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                        get<float>()
                                                        -
                                                        get<float>(currentID),
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
        }else if (currentType == "bool"){
            switch (lexer::determineOperatorType(binaryNode -> op)) {
                case lexer::TOK_NOT_EQAUL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                        get<bool>()
                                                        !=
                                                        get<bool>(currentID),
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<bool>()
                                                       ==
                                                       get<bool>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_AND:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<bool>()
                                                       &&
                                                       get<bool>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "0CurrentVariable";
                    break;
                case lexer::TOK_OR:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<bool>()
                                                       ||
                                                       get<bool>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<bool>()
                                                       >
                                                       get<bool>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<bool>()
                                                       <
                                                       get<bool>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<bool>()
                                                       >=
                                                       get<bool>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "0CurrentVariable";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<bool>()
                                                       <=
                                                       get<bool>(currentID),
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
        }else if (currentType == "string") {
            switch (lexer::determineOperatorType(binaryNode -> op)) {
                case lexer::TOK_NOT_EQAUL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<std::string>()
                                                       !=
                                                       get<std::string>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       get<std::string>()
                                                       ==
                                                       get<std::string>(currentID),
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_PLUS:
                    insert(interpreter::Variable<std::string>("string", "0CurrentVariable",
                                                              get<std::string>()
                                                              +
                                                              get<std::string>(currentID),
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
        }
        // Update Current ID
        currentID = "0CurrentVariable";
    }

    void Interpreter::visit(parser::ASTIdentifierNode *identifierNode) {
        // Build variable shells
        interpreter::Variable<int> i(identifierNode -> identifier);
        interpreter::Variable<float> f(identifierNode -> identifier);
        interpreter::Variable<bool> b(identifierNode -> identifier);
        interpreter::Variable<std::string> s(identifierNode -> identifier);
        // Check that a variable with this identifier exists
        auto resultI = find(i);
        if(found(resultI)) {
            // if identifier has been found
            // change current Type
            currentType = resultI -> second.type;
            // change current ID
            currentID = resultI -> second.identifier;
            // Then return
            return;
        }
        auto resultF = find(f);
        if(found(resultF)) {
            // if identifier has been found
            // change current Type
            currentType = resultF -> second.type;
            // change current ID
            currentID = resultF -> second.identifier;
            // Then return
            return;
        }
        auto resultB = find(b);
        if(found(resultB)) {
            // if identifier has been found
            // change current Type
            currentType = resultB -> second.type;
            // change current ID
            currentID = resultB -> second.identifier;
            // Then return
            return;
        }
        auto resultS = find(s);
        if(found(resultS)) {
            // if identifier has been found
            // change current Type
            currentType = resultS -> second.type;
            // change current ID
            currentID = resultS -> second.identifier;
            // Then return
            return;
        }
        // Variable hasn't been found (should never get here)
        throw std::runtime_error("Variable with identifier " + i.identifier + " called on line "
                                 + std::to_string(i.lineNumber) + " has not been declared.");
    }

    void Interpreter::visit(parser::ASTUnaryNode *unaryNode) {
        // visit the expression to get the type and id
        unaryNode -> exprNode -> accept(this);
        // now we check the type
        if(currentType == "int"){
            insert(interpreter::Variable<int>("int", "string", get<int>(currentID) * -1, unaryNode -> lineNumber));
        }else if(currentType == "float"){
            insert(interpreter::Variable<float>("float", "0CurrentVariable", get<float>(currentID) * -1, unaryNode -> lineNumber));
        }else if(currentType == "bool"){
            insert(interpreter::Variable<bool>("bool", "0CurrentVariable", ! get<bool>(currentID), unaryNode -> lineNumber));
        }else{
            // should get here
            throw std::runtime_error("Expression on line " + std::to_string(unaryNode -> lineNumber)
                                     + " has incorrect operator " + unaryNode -> op
                                     + " acting for expression of type " + currentType);
        }
    }

    void Interpreter::visit(parser::ASTFunctionCallNode *functionCallNode) {
        // Generate Function
        interpreter::Function f(functionCallNode -> identifier -> identifier);
        // find actual function
        auto result = find(f);
        if(!found(result)) {
            // Should never get here
            throw std::runtime_error("Function with identifier " + f.identifier + " called on line "
                                     + std::to_string(f.lineNumber) + " has not been declared.");
        }
        f = result->second;
        // go over the function parameters
        // and make sure to update these variables according to the parameters passed
        for (int i = 0; i < functionCallNode -> parameters.size(); ++i){
            // this visit will check if the variables exist
            functionCallNode -> parameters.at(i) -> accept(this);
            // This visit updates the currentID and currentType
            // store current ID so that we dont need to visit the parameters again to pop their values
            toPop.emplace_back(std::make_pair(currentType, f.paramIDs.at(i)));
            if (currentType == "int"){
                /* Update the currentID variable by emplacing back
                 * to f.paramIDs.at(i) variable
                 * what is found inside the variable with identifier currentID
                 * which we got from visiting the parameter expression
                 * this temporarily overwrites any global variable
                 * Once the block is function block is visited we pop back these variables to clear memory
                */
                insert(interpreter::Variable<int>("int", f.paramIDs.at(i), get<int>(currentID), functionCallNode -> lineNumber));
            }else if (currentType == "float"){
                insert(interpreter::Variable<float>("float", f.paramIDs.at(i), get<float>(currentID), functionCallNode -> lineNumber));
            }else if (currentType == "bool"){
                insert(interpreter::Variable<bool>("bool", f.paramIDs.at(i), get<bool>(currentID), functionCallNode -> lineNumber));
            }else if (currentType == "string"){
                insert(interpreter::Variable<std::string>("string", f.paramIDs.at(i), get<std::string>(currentID), functionCallNode -> lineNumber));
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
            if (pair.first == "int"){
                pop_back<int>(pair.second);
            }else if (pair.first == "float"){
                pop_back<float>(pair.second);
            }else if (pair.first == "bool"){
                pop_back<bool>(pair.second);
            }else if (pair.first == "string"){
                pop_back<std::string>(pair.second);
            }
        }
        toPop = std::vector<std::pair<std::string, std::string>>();
    }
    // Expressions

    // Statements

    void Interpreter::visit(parser::ASTSFunctionCallNode *sFunctionCallNode) {
        // Generate Function
        interpreter::Function f(sFunctionCallNode -> identifier -> identifier);
        // find actual function
        auto result = find(f);
        if(! found(result)) {
            // Should never get here
            throw std::runtime_error("Function with identifier " + f.identifier + " called on line "
                                     + std::to_string(f.lineNumber) + " has not been declared.");
        }
        f = result->second;
        // go over the function parameters
        // and make sure to update these variables according to the parameters passed
        for (int i = 0; i < sFunctionCallNode -> parameters.size(); ++i){
            // this visit will check if the variables exist
            sFunctionCallNode -> parameters.at(i) -> accept(this);
            // This visit updates the currentID and currentType
            // store current ID so that we dont need to visit the parameters again to pop their values
            toPop.emplace_back(std::make_pair(currentType, currentID));
            if (currentType == "int"){
                /* Update the currentID variable by emplacing back
                 * to f.paramIDs.at(i) variable
                 * what is found inside the variable with identifier currentID
                 * which we got from visiting the parameter expression
                 * this temporarily overwrites any global variable
                 * Once the block is function block is visited we pop back these variables to clear memory
                */
                insert(interpreter::Variable<int>("int", f.paramIDs.at(i), get<int>(currentID), sFunctionCallNode -> lineNumber));
            }else if (currentType == "float"){
                insert(interpreter::Variable<float>("float", f.paramIDs.at(i), get<float>(currentID), sFunctionCallNode -> lineNumber));
            }else if (currentType == "bool"){
                insert(interpreter::Variable<bool>("bool", f.paramIDs.at(i), get<bool>(currentID), sFunctionCallNode -> lineNumber));
            }else if (currentType == "string"){
                insert(interpreter::Variable<std::string>("string", f.paramIDs.at(i), get<std::string>(currentID), sFunctionCallNode -> lineNumber));
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
            if (pair.first == "int"){
                pop_back<int>(pair.second);
            }else if (pair.first == "float"){
                pop_back<float>(pair.second);
            }else if (pair.first == "bool"){
                pop_back<bool>(pair.second);
            }else if (pair.first == "string"){
                pop_back<std::string>(pair.second);
            }
        }
        toPop = std::vector<std::pair<std::string, std::string>>();
    }

    void Interpreter::visit(parser::ASTDeclarationNode *declarationNode) {
        // We dont visit the identifier as this would produce an error as the interpreter expects
        // a variable with identifier as provided to exist
        // instead we directly assign the variable when the interpreter::Variable is created
//        declarationNode -> identifier -> accept(this);

        // Visit the expression to get the current Type and Current Id
        declarationNode -> exprNode -> accept(this);
        // Now we have an updated current type and id
        // Create a variable with this information

        // Ensure interpreter/semantic is working as intended by comparing the type in the AST and the current TYPE
        if(declarationNode -> type != currentType){
            throw std::runtime_error("Types don't match between declaration and expression on line "
                                     + std::to_string(declarationNode -> lineNumber) + ".");
        }
        // Insert the new variable
        if(currentType == "int"){
            insert (
                    interpreter::Variable<int>(currentType, declarationNode -> identifier -> identifier, get<int>(currentID), declarationNode -> lineNumber)
            );
        }else if(currentType == "float"){
            insert (
                    interpreter::Variable<float>(currentType, declarationNode -> identifier -> identifier, get<float>(currentID), declarationNode -> lineNumber)
            );
        }else if(currentType == "bool"){
            insert (
                    interpreter::Variable<bool>(currentType, declarationNode -> identifier -> identifier, get<bool>(currentID), declarationNode -> lineNumber)
            );
        }else if(currentType == "string"){
            insert (
                    interpreter::Variable<std::string>(currentType, declarationNode -> identifier -> identifier, get<std::string>(currentID), declarationNode -> lineNumber)
            );
        }

        if(function){
            toPop.emplace_back(std::make_pair(currentType, declarationNode -> identifier -> identifier));
        }
    }

    void Interpreter::visit(parser::ASTAssignmentNode *assignmentNode) {
        // We visit the identifier node now as we expect the variable to exist
        assignmentNode -> identifier -> accept(this);
        // We can update the type and identifier local variables
        // These two variables define the found variable
        std::string type = currentType;
        std::string id = currentID;
        // Visit the expression to get the current Type and Current Id
        assignmentNode -> exprNode -> accept(this);
        // Now we have an updated current type and id
        // These two variables define what we will give id
        // Replace the variable value
        // Replacement can be done by popping then inserting
        if(currentType == "int"){
            get<int>(id);
            insert (
                    interpreter::Variable<int>(type, id, get<int>(currentID), assignmentNode -> lineNumber)
            );
        }else if(currentType == "float"){
            get<float>(id);
            insert (
                    interpreter::Variable<float>(type, id, get<float>(currentID), assignmentNode -> lineNumber)
            );
        }else if(currentType == "bool"){
            get<bool>(id);
            insert (
                    interpreter::Variable<bool>(type, id, get<bool>(currentID), assignmentNode -> lineNumber)
            );
        }else if(currentType == "string"){
            get<std::string>(id);
            insert (
                    interpreter::Variable<std::string>(type, id, get<std::string>(currentID), assignmentNode -> lineNumber)
            );
        }
        if(function){
            toPop.emplace_back(std::make_pair(currentType, assignmentNode -> identifier -> identifier));
        }
    }

    void Interpreter::visit(parser::ASTPrintNode *printNode) {
        // Visit expression node to get current type
        printNode -> exprNode -> accept(this);

        if(currentType == "int"){
            std::cout << get<int>(currentID) << std::endl;
        }else if(currentType == "float"){
            std::cout << get<float>(currentID) << std::endl;
        }else if(currentType == "bool"){
            std::cout << (get<bool>(currentID) ? "true" : "false") << std::endl;
        }else if(currentType == "string"){
            std::cout << get<std::string>(currentID) << std::endl;
        }
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
        if (get<bool>(currentID)){
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

        while(get<bool>(currentID)){
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

        while(get<bool>(currentID)){
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
    }
    // Statements
}