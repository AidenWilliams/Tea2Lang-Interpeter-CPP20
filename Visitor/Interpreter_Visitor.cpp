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
    }

    void Interpreter::visit(parser::ASTLiteralNode<float> *literalNode) {
        interpreter::Variable<float> v("float", "literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        floatTable.pop_back("literal");
        floatTable.insert(v);
        currentType = "float";
        currentID = "literal";
    }

    void Interpreter::visit(parser::ASTLiteralNode<bool> *literalNode) {
        interpreter::Variable<bool> v("bool", "literal", false, literalNode -> val, literalNode -> lineNumber);
        // remove previous literal
        boolTable.pop_back("literal");
        boolTable.insert(v);
        currentType = "bool";
        currentID = "literal";
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

    }

    void Interpreter::visit(parser::ASTArrayLiteralNode *arrayLiteralNode) {
        array = true;
        // create array
        if(currentType == "int"){
            std::vector<int> arr;
            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(intTable.get(currentID).latestValue);
            }
            interpreter::Variable<std::vector<int>> a("int", "literal", true, arr, arrayLiteralNode->lineNumber);
        }else if(currentType == "float"){
            std::vector<float> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(floatTable.get(currentID).latestValue);
            }
        }else if(currentType == "bool"){
            std::vector<bool> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(boolTable.get(currentID).latestValue);
            }
        }else if(currentType == "string"){
            std::vector<std::string> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(stringTable.get(currentID).latestValue);
            }
        }else if(currentType == "char"){
            std::vector<char> arr;

            for(const auto& item : arrayLiteralNode->expressions){
                item->accept(this);
                arr.emplace_back(charTable.get(currentID).latestValue);
            }
        }
        currentType = "object";
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
                                                      intTable.get(currentID).latestValue,
                                                      binaryNode -> lineNumber));
        }else if(currentType == "float"){
            floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                      false,
                                                      floatTable.get(currentID).latestValue,
                                                      binaryNode -> lineNumber));
        }else if(currentType == "bool"){
            boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                      false,
                                                      boolTable.get(currentID).latestValue,
                                                      binaryNode -> lineNumber));
        }else if(currentType == "string"){
            stringTable.insert(interpreter::Variable<std::string>("string", "0CurrentVariable",
                                                      false,
                                                      stringTable.get(currentID).latestValue,
                                                      binaryNode -> lineNumber));
        }else if(currentType == "char"){
            charTable.insert(interpreter::Variable<char>("char", "0CurrentVariable",
                                                      false,
                                                      charTable.get(currentID).latestValue,
                                                      binaryNode -> lineNumber));
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
                                                       intTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       ==
                                                       intTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       >
                                                       intTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       <
                                                       intTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                       >=
                                                       intTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                      <=
                                                      intTable.get(currentID).latestValue,
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
                                                       intTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                case lexer::TOK_ASTERISK:
                    intTable.insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                      *
                                                      intTable.get(currentID).latestValue,
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
                                                      intTable.get(currentID).latestValue,
                                                      binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "int";
                    break;
                case lexer::TOK_MINUS:
                    intTable.insert(interpreter::Variable<int>("int", "0CurrentVariable",
                                                       false,
                                                       intTable.get().latestValue
                                                      -
                                                      intTable.get(currentID).latestValue,
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
                                                       floatTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       ==
                                                       floatTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       >
                                                       floatTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       <
                                                       floatTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       >=
                                                       floatTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                       <=
                                                       floatTable.get(currentID).latestValue,
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
                                                       floatTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                case lexer::TOK_ASTERISK:
                    floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                        *
                                                        floatTable.get(currentID).latestValue,
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
                                                        floatTable.get(currentID).latestValue,
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "float";
                    break;
                case lexer::TOK_MINUS:
                    floatTable.insert(interpreter::Variable<float>("float", "0CurrentVariable",
                                                       false,
                                                       floatTable.get().latestValue
                                                        -
                                                        floatTable.get(currentID).latestValue,
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
                                                        boolTable.get(currentID).latestValue,
                                                        binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       ==
                                                       boolTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_AND:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       &&
                                                       boolTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "0CurrentVariable";
                    break;
                case lexer::TOK_OR:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       ||
                                                       boolTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       >
                                                       boolTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_LESS_THAN:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       <
                                                       boolTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       >=
                                                       boolTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "0CurrentVariable";
                    break;
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       boolTable.get().latestValue
                                                       <=
                                                       boolTable.get(currentID).latestValue,
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
                                                       stringTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_EQAUL_TO:
                    boolTable.insert(interpreter::Variable<bool>("bool", "0CurrentVariable",
                                                       false,
                                                       stringTable.get().latestValue
                                                       ==
                                                       stringTable.get(currentID).latestValue,
                                                       binaryNode -> lineNumber));
                    // Update Current Type to the that of the type being inserted
                    currentType = "bool";
                    break;
                case lexer::TOK_PLUS:
                    stringTable.insert(interpreter::Variable<std::string>("string", "0CurrentVariable",
                                                              false,
                                                              stringTable.get().latestValue
                                                              +
                                                              stringTable.get(currentID).latestValue,
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
        interpreter::Variable<char> c(identifierNode -> identifier);
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
        // Variable hasn't been found (should never get here)
        throw std::runtime_error("Variable with identifier " + i.identifier + " called on line "
                                 + std::to_string(i.lineNumber) + " has not been declared.");
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
            if(currentType == "int"){
                /* Update the currentID variable by emplacing back
                 * to f.paramIDs.at(i) variable
                 * what is found inside the variable with identifier currentID
                 * which we got from visiting the parameter expression
                 * this temporarily overwrites any global variable
                 * Once the block is function block is visited we pop back these variables to clear memory
                */
                intTable.insert(interpreter::Variable<int>("int", f.paramIDs.at(i), false, intTable.get(currentID).latestValue, functionCallNode -> lineNumber));
            }else if(currentType == "float"){
                floatTable.insert(interpreter::Variable<float>("float", f.paramIDs.at(i), false, floatTable.get(currentID).latestValue, functionCallNode -> lineNumber));
            }else if(currentType == "bool"){
                boolTable.insert(interpreter::Variable<bool>("bool", f.paramIDs.at(i), false, boolTable.get(currentID).latestValue, functionCallNode -> lineNumber));
            }else if(currentType == "string"){
                stringTable.insert(interpreter::Variable<std::string>("string", f.paramIDs.at(i), false, stringTable.get(currentID).latestValue, functionCallNode -> lineNumber));
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
            if(pair.first == "int"){
                intTable.pop_back(pair.second);
            }else if(pair.first == "float"){
                floatTable.pop_back(pair.second);
            }else if(pair.first == "bool"){
                boolTable.pop_back(pair.second);
            }else if(pair.first == "string"){
                stringTable.pop_back(pair.second);
            }else if(pair.first == "char"){
                charTable.pop_back(pair.second);
            }
        }
        toPop = std::vector<std::pair<std::string, std::string>>();
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
            if(currentType == "int"){
                /* Update the currentID variable by emplacing back
                 * to f.paramIDs.at(i) variable
                 * what is found inside the variable with identifier currentID
                 * which we got from visiting the parameter expression
                 * this temporarily overwrites any global variable
                 * Once the block is function block is visited we pop back these variables to clear memory
                */
                intTable.insert(interpreter::Variable<int>("int", f.paramIDs.at(i), false, intTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
            }else if(currentType == "float"){
                floatTable.insert(interpreter::Variable<float>("float", f.paramIDs.at(i), false, floatTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
            }else if(currentType == "bool"){
                boolTable.insert(interpreter::Variable<bool>("bool", f.paramIDs.at(i), false, boolTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
            }else if(currentType == "string"){stringTable.insert(interpreter::Variable<std::string>
                ("string", f.paramIDs.at(i), false, stringTable.get(currentID).latestValue, sFunctionCallNode -> lineNumber));
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
            if(pair.first == "int"){
                intTable.pop_back(pair.second);
            }else if(pair.first == "float"){
                floatTable.pop_back(pair.second);
            }else if(pair.first == "bool"){
                boolTable.pop_back(pair.second);
            }else if(pair.first == "string"){
                stringTable.pop_back(pair.second);
            }else if(pair.first == "char"){
                charTable.pop_back(pair.second);
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
            intTable.insert (
                    interpreter::Variable<int>(currentType, declarationNode -> identifier -> identifier, array, intTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "float"){
            floatTable.insert (
                    interpreter::Variable<float>(currentType, declarationNode -> identifier -> identifier, array, floatTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "bool"){
            boolTable.insert (
                    interpreter::Variable<bool>(currentType, declarationNode -> identifier -> identifier, array, boolTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "string"){
            stringTable.insert (
                    interpreter::Variable<std::string>(currentType, declarationNode -> identifier -> identifier, array, stringTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }else if(currentType == "char"){
            charTable.insert (
                    interpreter::Variable<char>(currentType, declarationNode -> identifier -> identifier, array, charTable.get(currentID).latestValue, declarationNode -> lineNumber)
            );
        }
        array = false;

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
            toPop.emplace_back(std::make_pair(currentType, assignmentNode -> identifier -> identifier));
        }
    }

    void Interpreter::visit(parser::ASTPrintNode *printNode) {
        // Visit expression node to get current type
        printNode -> exprNode -> accept(this);

        if(currentType == "int"){
            std::cout << intTable.get(currentID).latestValue << std::endl;
        }else if(currentType == "float"){
            std::cout << floatTable.get(currentID).latestValue << std::endl;
        }else if(currentType == "bool"){
            std::cout << (boolTable.get(currentID).latestValue ? "true" : "false") << std::endl;
        }else if(currentType == "string"){
            std::cout << stringTable.get(currentID).latestValue << std::endl;
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
        auto save = std::make_pair(currentType, currentID);
        // find the return
        for (auto it = toPop.begin(); it != toPop.end(); ++it){
            if(*it == save){
                toPop.erase(it);
                if(it == toPop.end()) break;
            }
        }

    }

    void Interpreter::visit(parser::ASTStructNode *structNode) {

    }
    // Statements
}