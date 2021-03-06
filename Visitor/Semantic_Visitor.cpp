//
// Created by Aiden Williams on 26/06/2021.
//

#include "Semantic_Visitor.h"

namespace semantic{
    // Struct
    void Struct::insert(const Variable& v) {
        variables.emplace_back(v);
    }
    void Struct::insert(const Function& f) {
        functions.emplace_back(f);
    }
    void Struct::defineLineNumber(unsigned int lineNumber){
        this->lineNumber = lineNumber;
    }
    //Struct
    // Semantic Scope
    bool Scope::insert(const Variable& v){
        if (v.type.empty()){
            throw VariableTypeException();
        }
        auto ret = variableTable.insert(std::pair<std::string, Variable>(v.identifier, v) );
        return ret.second;
    }

    bool Scope::insert(const Function& f){
        if (f.type.empty()){
            throw FunctionTypeException();
        }
        auto ret = functionTable.insert (std::pair<std::pair<std::string, std::vector<std::string>>, Function>
                                        (             std::make_pair(f.identifier, f.paramTypes),       f));
        return ret.second;
    }

    bool Scope::insert(const Struct& s){
        auto ret = structTable.insert(std::pair<std::string, Struct>(s.identifier, s) );
        return ret.second;
    }

    auto Scope::find(const Variable& v) {
        return variableTable.find(v.identifier);
    }

    auto Scope::find(const Function& f) {
        return functionTable.find( std::pair(f.identifier, f.paramTypes));
    }

    auto Scope::find(const Struct& s){
        return structTable.find(s.identifier);
    }

    void Scope::insertTo(const Struct& s, const Variable& v){
        auto result = find(s);
        if(!found(result)){
            throw StructInsertionException();
        }
        auto cpy(result -> second);
        // add the new value
        cpy.insert(v);
        // remove the result
        structTable.erase(result);
        // insert the copy
        insert(cpy);
    }
    void Scope::insertTo(const Struct& s, const Function& f){
        auto result = find(s);
        if(!found(result)){
            throw StructInsertionException();
        }
        auto cpy(result -> second);
        // add the new value
        cpy.insert(f);
        // remove the result
        structTable.erase(result);
        // insert the copy
        insert(cpy);
    }

    bool Scope::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Variable>> result) {
        return result != variableTable.end();
    }

    bool Scope::found(
            std::_Rb_tree_iterator<std::pair<const std::pair<std::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>, Function>>
            result) {
        return result != functionTable.end();
    }

    bool Scope::found(std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Struct>> result){
        return result != structTable.end();
    }

    bool Scope::erase(
            std::_Rb_tree_iterator<std::pair<const std::pair<std::basic_string<char, std::char_traits<char>, std::allocator<char>>, std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char>>>>, Function>>
            result) {
        if(found(result)){
            functionTable.erase(result);
            return true;
        }else{
            return false;
        }
    }
    // Semantic Scope
}

namespace visitor{

    // Semantic Analyses

    // Program
    void SemanticAnalyser::visit(parser::ASTProgramNode *programNode) {
        scopes.emplace_back(std::make_shared<semantic::Scope>(true));
        // For each statement, accept
        for(auto &statement : programNode -> statements)
            statement -> accept(this);
        scopes.pop_back();
    }
    // Program

    // Expressions
    // Literal visits change the currentType value
    void SemanticAnalyser::visit(parser::ASTLiteralNode<int> *literalNode) {
        currentType = "int";
    }

    void SemanticAnalyser::visit(parser::ASTLiteralNode<float> *literalNode) {
        currentType = "float";
    }

    void SemanticAnalyser::visit(parser::ASTLiteralNode<bool> *literalNode) {
        currentType = "bool";
    }

    void SemanticAnalyser::visit(parser::ASTLiteralNode<std::string> *literalNode) {
        currentType = "string";
    }

    void SemanticAnalyser::visit(parser::ASTLiteralNode<char> *literalNode) {
        currentType = "char";
    }

    void SemanticAnalyser::visit(parser::ASTArrayLiteralNode *arrayLiteralNode) {
        // start going over the items in the array
        auto _cType = currentType;
        int i = 0;
        for(auto &item : arrayLiteralNode->expressions){
            item->accept(this);
            if(i == 0 && _cType == "auto"){
                _cType = currentType;
            }
            // are the types still matching?
            if(_cType != currentType){
                throw std::runtime_error("The " + std::to_string(i) + " th item's type on line "
                                         + std::to_string(arrayLiteralNode->lineNumber) +" does not match "
                                         + "The previous type. The type should be "
                                         + _cType);
            }
            i++;
        }
    }

    void SemanticAnalyser::visit(parser::ASTBinaryNode *binaryNode) {
        // Go over left expression first
        binaryNode->left->accept(this);
        // store the currentType for the left expression
        std::string leftType(currentType);
        // Now go over the right expression
        binaryNode->right->accept(this);
        // store the currentType for the left expression
        std::string rightType(currentType);
        // type casting is not supported so both types need to be the same
        if (leftType != rightType) {
            // Variable hasn't been found in any scope
            throw std::runtime_error("Expression with left type " + leftType + " does not share a common right type "
                                     + rightType + " on line " + std::to_string(binaryNode->lineNumber)
                                     + ".\nImplicit and Automatic Typecasting is not supported by TeaLang.");
        }

        // check op type
        if (currentType == "string" || currentType == "char") {
            switch (lexer::determineOperatorType(binaryNode->op)) {
                // string accepted operators
                case lexer::TOK_NOT_EQAUL_TO:
                case lexer::TOK_EQAUL_TO:
                    // change current type to bool as a condition has been found
                    currentType = "bool";
                case lexer::TOK_PLUS:
                    // Valid tokens
                    break;
                default:
                    throw std::runtime_error("Expression on line " + std::to_string(binaryNode->lineNumber)
                                                + " has incorrect operator " + binaryNode->op
                                                + " acting between expressions of type " + currentType);
            }
        }else if (currentType == "int" || currentType == "float") {
            switch (lexer::determineOperatorType(binaryNode->op)) {
                // string accepted operators
                case lexer::TOK_NOT_EQAUL_TO:
                case lexer::TOK_EQAUL_TO:
                case lexer::TOK_MORE_THAN:
                case lexer::TOK_LESS_THAN:
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    // change current type to bool as a condition has been found
                    currentType = "bool";
                // int and float accepted operators
                case lexer::TOK_PLUS:
                case lexer::TOK_ASTERISK:
                case lexer::TOK_DIVIDE:
                case lexer::TOK_MINUS:
                    // Valid tokens
                    break;
                default:
                    throw std::runtime_error("Expression on line " + std::to_string(binaryNode->lineNumber)
                                             + " has incorrect operator " + binaryNode->op
                                             + " acting between expressions of type " + currentType);
            }
        }else if (currentType == "bool"){
            switch (lexer::determineOperatorType(binaryNode->op)) {
                case lexer::TOK_NOT_EQAUL_TO:
                case lexer::TOK_EQAUL_TO:
                case lexer::TOK_AND:
                case lexer::TOK_OR:
                case lexer::TOK_MORE_THAN:
                case lexer::TOK_LESS_THAN:
                case lexer::TOK_MORE_THAN_EQUAL_TO:
                case lexer::TOK_LESS_THAN_EQUAL_TO:
                    // current type is already bool
                    // Valid tokens
                    break;
                default:
                    throw std::runtime_error("Expression on line " + std::to_string(binaryNode->lineNumber)
                                             + " has incorrect operator " + binaryNode->op
                                             + " acting between expressions of type " + currentType);
            }
        }else{
            // structs (operator overloading is not a feature)
            throw std::runtime_error("Expression on line " + std::to_string(binaryNode->lineNumber)
                                     + " has incorrect operator " + binaryNode->op
                                     + " acting between expressions of type " + currentType);
        }
    }

    void SemanticAnalyser::visit(parser::ASTIdentifierNode *identifierNode) {
        // There are 2 cases here
        // one where this is a normal variable (i.e. no '.')
        // the other when the identifier is referencing another variable
        auto parent = parser::ASTIdentifierNode(identifierNode->identifier, identifierNode->getChild(), identifierNode->ilocExprNode, identifierNode->lineNumber);
        auto child = identifierNode->getChild();
        bool found = false;
        while(child != nullptr){
            // we have found a child
            // this means that the identifier of identifierNode must be a struct
            for(const auto& scope : scopes) {
                // First find the variable (remember identifierNode->identifier is a variable of a type)
                auto result = scope->find(semantic::Variable(parent.identifier));
                if(scope->found(result)) {
                    // we found it, now does it have a struct type?
                    if(!lexer::isStruct(result->second.type)){
                        throw std::runtime_error("Variable with identifier " + parent.identifier + " called on line "
                                                 + std::to_string(parent.lineNumber) + " is not a tlstruct object. Only"
                                                                                                "tlstruct objects can reference other variables or functions via the '.' operator.");
                    }
                    // get the struct
                    for(const auto& _scope : scopes) {
                        auto struct_result = _scope->find(semantic::Struct(result->second.type));
                        if(_scope->found(struct_result)) {
                            // found the struct
                            // go over its variables and verify child.identifier is there
                            for(auto var : struct_result->second.variables){
                                if(var.identifier == child->identifier) {
                                    //found
                                    found = true;
                                    currentType = var.type;
                                    break;
                                }
                            }
                            if(found) break;
                        }
                    }
                    if(found) {
                        found = false;
                    }else{
                        throw std::runtime_error("Variable with identifier " + child->identifier + " called on line "
                                                 + std::to_string(parent.lineNumber) + " is not a child object of "
                                                 + parent.identifier);
                    }
                    break;
                }
            }
            parent = parser::ASTIdentifierNode(child);
            child = child->getChild();
            if(child == nullptr)
                return;
        }

        // normal variable case
        // Build variable shell
        semantic::Variable v(identifierNode->getID());
        // Check that a variable with this identifier exists
        for(const auto& scope : scopes) {
            auto result = scope->find(v);
            if(scope->found(result)) {
                // if identifier has been found
                // change current Type
                currentType = result->second.type;
                // Then return
                return;
            }
        }
        // Variable hasn't been found in any scope
        throw std::runtime_error("Variable with identifier " + identifierNode->getID() + " called on line "
                                 + std::to_string(identifierNode->lineNumber) + " has not been declared.");
    }

    void SemanticAnalyser::visit(parser::ASTUnaryNode *unaryNode) {
        // Go over exprNode
        unaryNode -> exprNode -> accept(this);
        // Handle different cases
        if (currentType == "int" || currentType == "float") {
            if(unaryNode -> op != "-")
                throw std::runtime_error("Expression on line " + std::to_string(unaryNode->lineNumber)
                                         + " has incorrect operator " + unaryNode->op
                                         + " acting for expression of type " + currentType);
        }else if (currentType == "bool"){
            if(unaryNode -> op != "not")
                throw std::runtime_error("Expression on line " + std::to_string(unaryNode->lineNumber)
                                         + " has incorrect operator " + unaryNode->op
                                         + " acting for expression of type " + currentType);

        }else{
            // string char and structs (operator overloading is not a feature)
            throw std::runtime_error("Expression on line " + std::to_string(unaryNode->lineNumber)
                                     + " has incorrect operator " + unaryNode->op
                                     + " acting for expression of type " + currentType);
        }
    }

    void SemanticAnalyser::visit(parser::ASTFunctionCallNode *functionCallNode) {
        // Check parameters
        std::vector<std::string> paramTypes;
        for (const auto& param : functionCallNode->parameters){
            // this visit will check if the variables exist
            param->accept(this);
            // store the types (in order) so than we can confirm that the types are good
            // when the function is found
            paramTypes.emplace_back(currentType);
        }
        // Unfortunately we cant just visit the identifier because this doesnt carry over the paramTypes
        // of course we can just add another 'global' variable but it is much easier to copy the code
        // There are 2 cases here
        // one where this is a normal function (i.e. no '.')
        // the other when the identifier is referencing another variable
        auto parent = parser::ASTIdentifierNode(functionCallNode->identifier);
        auto child = parent.getChild();
        bool found = false;
        while(child != nullptr){
            // we have found a child
            // this means that the identifier of identifierNode must be a struct
            for(const auto& scope : scopes) {
                // First find the variable (remember identifierNode->identifier is a variable of a type)
                auto result = scope->find(semantic::Variable(parent.identifier));
                if(scope->found(result)) {
                    // we found it, now does it have a struct type?
                    if(!lexer::isStruct(result->second.type)){
                        throw std::runtime_error("Variable with identifier " + parent.identifier + " called on line "
                                                 + std::to_string(parent.lineNumber) + " is not a tlstruct object. Only"
                                                                                       "tlstruct objects can reference other variables or functions via the '.' operator.");
                    }
                    // get the struct
                    for(const auto& _scope : scopes) {
                        auto struct_result = _scope->find(semantic::Struct(result->second.type));
                        if(_scope->found(struct_result)) {
                            // found the struct
                            // go over its functions and verify child.identifier is there
                            for(auto func : struct_result->second.functions){
                                if(func.identifier == child->identifier && func.paramTypes == paramTypes) {
                                    //found
                                    found = true;
                                    currentType = func.type;
                                    break;
                                }
                            }
                            if(found) break;
                        }
                    }
                    if(found) {
                        found = false;
                    }else{
                        throw std::runtime_error("Function with identifier " + child->identifier + " called on line "
                                                 + std::to_string(parent.lineNumber) + " is not a child function of "
                                                 + parent.identifier);
                    }
                    break;
                }
            }
            parent = parser::ASTIdentifierNode(child);
            child = child->getChild();
            if(child == nullptr)
                return;
        }

        // normal function case
        // now generate the function object
        semantic::Function f(functionCallNode->identifier->getID(), paramTypes);
        // Now confirm this exists in the function table for any scope
        for(const auto& scope : scopes){
            auto result = scope->find(f);
            if(scope->found(result)) {
                // change current type to the function return type
                currentType = result->second.type;
                // start going over the parameters in the function
                for (int i = 0; i < result->second.paramTypes.size(); ++i){
                    if (result->second.paramTypes.at(i) != paramTypes.at(i)){
                        throw std::runtime_error("The " + std::to_string(i) + "th argument's type on line "
                                                 + std::to_string(f.lineNumber) +" does not match "
                                                 + f.identifier + "'s argument signature. The type should be "
                                                 + result->second.paramTypes.at(i));
                    }
                }
                // all params match
                return;
            }
        }
        // Function hasn't been found in any scope
        throw std::runtime_error("Function with identifier " + functionCallNode->identifier->getID() + " called on line "
                                 + std::to_string(functionCallNode->lineNumber) + " has not been declared.");
    }
    // Expressions

    // Statements
    void SemanticAnalyser::visit(parser::ASTSFunctionCallNode *sFunctionCallNode) {
        // Check parameters
        std::vector<std::string> paramTypes;
        for (const auto& param : sFunctionCallNode->parameters){
            // this visit will check if the variables exist
            param->accept(this);
            // store the types (in order) so than we can confirm that the types are good
            // when the function is found
            paramTypes.emplace_back(currentType);
        }
        // There are 2 cases here
        // one where this is a normal function (i.e. no '.')
        // the other when the identifier is referencing another variable
        auto parent = parser::ASTIdentifierNode(sFunctionCallNode->identifier);
        auto child = parent.getChild();
        bool found = false;
        while(child != nullptr){
            // we have found a child
            // this means that the identifier of identifierNode must be a struct
            for(const auto& scope : scopes) {
                // First find the variable (remember identifierNode->identifier is a variable of a type)
                auto result = scope->find(semantic::Variable(parent.identifier));
                if(scope->found(result)) {
                    // we found it, now does it have a struct type?
                    if(!lexer::isStruct(result->second.type)){
                        throw std::runtime_error("Variable with identifier " + parent.identifier + " called on line "
                                                 + std::to_string(parent.lineNumber) + " is not a tlstruct object. Only"
                                                                                       "tlstruct objects can reference other variables or functions via the '.' operator.");
                    }
                    // get the struct
                    for(const auto& _scope : scopes) {
                        auto struct_result = _scope->find(semantic::Struct(result->second.type));
                        if(_scope->found(struct_result)) {
                            // found the struct
                            // go over its functions and verify child.identifier is there
                            for(auto func : struct_result->second.functions){
                                if(func.identifier == child->identifier && func.paramTypes == paramTypes) {
                                    //found
                                    found = true;
                                    currentType = func.type;
                                    break;
                                }
                            }
                            if(found) break;
                        }
                    }
                    if(found) {
                        found = false;
                    }else{
                        throw std::runtime_error("Function with identifier " + child->identifier + " called on line "
                                                 + std::to_string(parent.lineNumber) + " is not a child object of "
                                                 + parent.identifier);
                    }
                    break;
                }
            }
            parent = parser::ASTIdentifierNode(child);
            child = child->getChild();
            if(child == nullptr)
                return;
        }

        // normal function case
        // now generate the function object
        semantic::Function f(sFunctionCallNode->identifier->getID(), paramTypes);
        // Now confirm this exists in the function table for any scope
        for(const auto& scope : scopes){
            auto result = scope->find(f);
            if(scope->found(result)) {
                // change current type to the function return type
                currentType = result->second.type;
                // start going over the parameters in the function
                for (int i = 0; i < result->second.paramTypes.size(); ++i){
                    if (result->second.paramTypes.at(i) != paramTypes.at(i)){
                        throw std::runtime_error("The " + std::to_string(i) + "th argument's type on line "
                                                 + std::to_string(f.lineNumber) +" does not match "
                                                 + f.identifier + "'s argument signature. The type should be "
                                                 + result->second.paramTypes.at(i));
                    }
                }
                // all params match
                return;
            }
        }
        // Function hasn't been found in any scope
        throw std::runtime_error("Function with identifier " + sFunctionCallNode->identifier->getID() + " called on line "
                                 + std::to_string(sFunctionCallNode->lineNumber) + " has not been declared.");
    }

    void SemanticAnalyser::visit(parser::ASTDeclarationNode *declarationNode) {
        // Generate Variable
        semantic::Variable v(declarationNode->type, declarationNode->identifier->getID(),
                declarationNode->identifier->ilocExprNode != nullptr, declarationNode->lineNumber);
        // Check current scope
        auto scope = scopes.back();
        // Try to insert v
        auto result = scope->find(v);
        // compare the found key and the actual key
        // if identical than the variable is already declared
        if(scope->found(result)){
            // The variable has already been declared in the current scope
            throw std::runtime_error("Variable with identifier " + declarationNode->identifier->getID() + " declared on line "
                                     + std::to_string(declarationNode->lineNumber) + " already declared on line "
                                     + std::to_string(result->second.lineNumber));
        }
        // by changing the current type we help to init an array literal
        // if not an array this will be overwritten by the visit
        // if not visited then this should be auto or struct
        currentType = declarationNode->type;
        // There are 2 cases here
        // One where there is a default constructor (structs & arrays)
        // and the other is the classic Tealang
        // the parser sets exprNode to an expression if an = is found
        // this allows for the construction of structs and arrays
        // if = is not found than the variable must be either a struct or an array with [SIZE]
        // but this is handled by the parser so here we only need to check if exprNode is null
        // Go check the expression node
        // This will change the current type
        if(declarationNode->exprNode != nullptr){
            declarationNode->exprNode->accept(this);
        }

        // handle auto here
        if(declarationNode->type == "auto" || lexer::isStruct(declarationNode->type)){
            if(currentType == "auto"){
                throw std::runtime_error("Variable " + v.identifier + " was declared of type auto on line "
                                         + std::to_string(v.lineNumber) + " but has not been assigned a value of a defined type.");

            }
            v.type = currentType;

        }
        // set the currentType to declarationNode->type
        // this will help initialise array literals
        // Check current type with the declaration type
        // since the language does not perform any implicit/automatic typecast (as said in spec)
        // auto is handled at the interpreter
        if(declarationNode->type == currentType || declarationNode->type == "auto"){
            scope->insert(v);
            // add this to the struct as well (if we are in a struct)
            if(!structID.empty()){
                structScope->insertTo(semantic::Struct(structID), v);
            }
        }else{
            // throw an error since type casting is not supported
            throw std::runtime_error("Variable " + v.identifier + " was declared of type " + v.type + " on line "
                                     + std::to_string(v.lineNumber) + " but has been assigned invalid value of type "
                                     + currentType + ".\nImplicit and Automatic Typecasting is not supported by TeaLang.");
        }
    }

    void SemanticAnalyser::visit(parser::ASTAssignmentNode *assignmentNode) {
        // Get the exprNode type
        assignmentNode->exprNode->accept(this);
        /* Check for a variable with:
         * identifier = assignmentNode.identifier
         * type = currentType
         */
        // visit the identifier
        assignmentNode->identifier->accept(this);
    }

    void SemanticAnalyser::visit(parser::ASTPrintNode *printNode) {
        // Get the exprNode type
        printNode -> exprNode -> accept(this);
    }

    void SemanticAnalyser::visit(parser::ASTBlockNode *blockNode) {
        // Create new scope
        scopes.emplace_back(std::make_shared<semantic::Scope>());
        // Visit each statement in the block
        for(auto &statement : blockNode -> statements)
            statement -> accept(this);
        // Close scope
        scopes.pop_back();
    }

    void SemanticAnalyser::visit(parser::ASTIfNode *ifNode) {
        // Get the condition type
        ifNode -> condition -> accept(this);
        // Make sure it is boolean
        if(currentType != "bool")
            throw std::runtime_error("Invalid if-condition on line " + std::to_string(ifNode->lineNumber)
                                     + ", expected boolean expression.");
        // Store the returns as a return might be in the if
        bool ret = returns;
        // Check the if block
        ifNode -> ifBlock -> accept(this);
        if(ret != returns){
            // reset returns if it changed
            returns = ret;
        }
        // If there is an else block, check it too
        if(ifNode -> elseBlock != nullptr)
            ifNode -> elseBlock -> accept(this);
    }

    void SemanticAnalyser::visit(parser::ASTForNode *forNode) {
        // Create new scope for loop params
        // This allows the creation of a new variable only used by the loop
        scopes.emplace_back(std::make_shared<semantic::Scope>());
        // First go over the declaration
        if(forNode -> declaration != nullptr )
            forNode -> declaration -> accept(this);
        // Get the condition type
        forNode -> condition -> accept(this);
        // Make sure it is boolean
        if(currentType != "bool")
            throw std::runtime_error("Invalid for-condition on line " + std::to_string(forNode->lineNumber)
                                     + ", expected boolean expression.");
        // Now go over the assignment
        if(forNode -> assignment != nullptr)
            forNode -> assignment -> accept(this);
        // Now go over the loop block
        forNode -> loopBlock ->accept(this);
        // Close loop scope
        // This discards any declared variable in the for(;;) section
        scopes.pop_back();
    }

    void SemanticAnalyser::visit(parser::ASTWhileNode *whileNode) {
        // Get the condition type
        whileNode -> condition -> accept(this);
        // Make sure it is boolean
        if(currentType != "bool")
            throw std::runtime_error("Invalid while-condition on line " + std::to_string(whileNode->lineNumber)
                                     + ", expected boolean expression.");
        // Check the while block
        whileNode -> loopBlock -> accept(this);
    }

    void SemanticAnalyser::visit(parser::ASTFunctionDeclarationNode *functionDeclarationNode) {
        // If current scope is not global then do not allow declaration
        auto scope = scopes.back();
        if (!scope->isFunctionDeclarable()){
            throw std::runtime_error("Tried declaring function with identifier " + functionDeclarationNode->identifier->getID()
                                     + " in a non-global scope.");
        }
        // Create new scope for function params
        // This allows the creation of a new variables when they are params
        scopes.emplace_back(std::make_shared<semantic::Scope>());
        // Generate Function
        // First get the param types vector
        std::vector<std::string> paramTypes;
        for (const auto& param : functionDeclarationNode->parameters){
            paramTypes.emplace_back(param.second);
            // While going over the types add these to the new scope // arrau or not here it is irrelevant
            scopes.back()->insert(semantic::Variable(param.second, param.first, true, functionDeclarationNode->lineNumber));
        }
        // NOTE: The scope variable is still viewing the global scope
        // now generate the function object
        semantic::Function f(functionDeclarationNode->type, functionDeclarationNode->identifier->getID(), paramTypes, functionDeclarationNode->lineNumber);
        // Try to insert f
        auto result = scope->find(f);
        // compare the found key and the actual key
        // if identical than the function is already declared
        if(scope->found(result)){
            // The variable has already been declared in the current scope
            throw std::runtime_error("Function with identifier " + functionDeclarationNode->identifier->getID() + " declared on line "
                                     + std::to_string(functionDeclarationNode->lineNumber) + " already declared on line "
                                     + std::to_string(result->second.lineNumber));
        }
        // insert function to the function table, this allows for recursion to happen
        scope->insert(f);
        // Go check the block node
        returns = false;
        functionDeclarationNode->functionBlock->accept(this);
        // confirm function has a return and that the return type is as defined in the declaration node
        if(!returns){
            throw std::runtime_error("Function with identifier " + functionDeclarationNode->identifier->getID() + " declared on line "
                                     + std::to_string(functionDeclarationNode->lineNumber) + " does not have a return statement.");
        }
        // Check that the return type matches with the function type
        if(functionDeclarationNode->type != currentType) {
            if(functionDeclarationNode->type == "auto"){
                // remove function and re insert it with the new type
                scope->erase(scope->find(f));
                scope->insert(semantic::Function(currentType, functionDeclarationNode->identifier->getID(), paramTypes, functionDeclarationNode->lineNumber));
                // add this to the struct as well (if we are in a struct)
                if(!structID.empty()){
                    structScope->insertTo(semantic::Struct(structID), f);
                }
            }else{
                // Check current type with the declaration type
                // since the language does not perform any implicit/automatic typecast (as said in spec)
                throw std::runtime_error("Function " + functionDeclarationNode->identifier->getID() + " was declared of type " + f.type + " on line "
                                         + std::to_string(functionDeclarationNode->lineNumber) + " but has been assigned invalid value of type "
                                         + currentType +
                                         ".\nImplicit and Automatic Typecasting is not supported by TeaLang.");
            }
        }else{
            // add this to the struct as well (if we are in a struct)
            if(!structID.empty()){
                structScope->insertTo(semantic::Struct(structID), f);
            }
        }
        // Close function scope
        // This discards any declared variable in the foo() section
        scopes.pop_back();
    }

    void SemanticAnalyser::visit(parser::ASTReturnNode *returnNode) {
        // Ensure returns is false
        if(returns){
            throw semantic::ReturnsException();
        }
        // Update current expression and update currentType
        returnNode -> exprNode -> accept(this);
        // mark returns as true
        returns = true;
    }

    void SemanticAnalyser::visit(parser::ASTStructNode *structNode) {
        // get current scope
        auto scope = scopes.back();
        // Generate a dummy struct object
        semantic::Struct s(structNode->identifier->getID());
        // Try to insert f
        auto result = scope->find(s);
        // compare the found key and the actual key
        // if identical than the struct is already declared
        if(scope->found(result)){
            // The variable has already been declared in the current scope
            throw std::runtime_error("Struct with identifier " + structNode->identifier->getID() + " declared on line "
                                     + std::to_string(structNode->lineNumber) + " already declared on line "
                                     + std::to_string(result->second.lineNumber));
        }
        // insert struct to the struct table, this allows us to build the variable and function lists in the struct
        // inside the block
        s.defineLineNumber(structNode->lineNumber);
        //insert scope
        scope->insert(s);
        structID = s.identifier;
        structScope = scope;
        scopes.emplace_back(std::make_shared<semantic::Scope>(true));
        // Visit each statement in the block
        for(auto &statement : structNode->structBlock -> statements)
            statement -> accept(this);
        // Close scope
        structID = "";
        structScope = nullptr;
        scopes.pop_back();
    }
    // Statements
}