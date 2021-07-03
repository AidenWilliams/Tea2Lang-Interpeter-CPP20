//
// Created by Aiden Williams on 26/06/2021.
//
/*
 *

 */

#include "Semantic_Visitor.h"

namespace visitor{
    // Semantic Scope
    //TODO: Definitely needs to be tested out
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
        auto ret = functionTable.insert(std::pair<std::string, Function>(f.identifier, f) );
        return ret.second;
    }

    std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Variable>>
    Scope::find(const Variable& v) {
        return variableTable.find(v.identifier);
    }

    std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Function>>
    Scope::find(const Function& f) {
        return functionTable.find(f.identifier);
    }

    bool Scope::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Variable>> result) {
        return result != variableTable.end();
    }

    bool Scope::found(
            std::_Rb_tree_iterator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char>>, Function>> result) {
        return result != functionTable.end();;
    }
    // Semantic Scope

    // Semantic Analyses

    // Program
    void SemanticAnalyser::visit(parser::ASTProgramNode *programNode) {
        scopes.emplace_back(std::make_shared<Scope>(true));
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
        if (currentType == "string") {
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
        }
    }

    void SemanticAnalyser::visit(parser::ASTIdentifierNode *identifierNode) {
        // Build variable shell
        Variable v(identifierNode->identifier);
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
        throw std::runtime_error("Variable with identifier " + v.identifier + " called on line "
                                 + std::to_string(v.lineNumber) + " has not been declared.");
    }

    void SemanticAnalyser::visit(parser::ASTUnaryNode *unaryNode) {
        // Go over exprNode
        unaryNode -> exprNode -> accept(this);
        // Handle different cases
        if (currentType == "string") {
            throw std::runtime_error("Expression on line " + std::to_string(unaryNode->lineNumber)
                                     + " has incorrect operator " + unaryNode->op
                                     + " acting for expression of type " + currentType);
        }else if (currentType == "int" || currentType == "float") {
            if(unaryNode -> op != "-")
                throw std::runtime_error("Expression on line " + std::to_string(unaryNode->lineNumber)
                                         + " has incorrect operator " + unaryNode->op
                                         + " acting for expression of type " + currentType);
        }else if (currentType == "bool"){
            if(unaryNode -> op != "not")
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
        // Generate Function
        Function f(functionCallNode->identifier->identifier);
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
        throw std::runtime_error("Function with identifier " + f.identifier + " called on line "
                                 + std::to_string(f.lineNumber) + " has not been declared.");
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
        // now generate the function object
        Function f(sFunctionCallNode->identifier->identifier);
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
        throw std::runtime_error("Function with identifier " + f.identifier + " called on line "
                                 + std::to_string(f.lineNumber) + " has not been declared.");
    }

    void SemanticAnalyser::visit(parser::ASTDeclarationNode *declarationNode) {
        // Generate Variable
        Variable v(declarationNode->type, declarationNode->identifier->identifier, declarationNode->lineNumber);
        // Check current scope
        auto scope = scopes.back();
        // Try to insert v
        auto result = scope->find(v);
        // compare the found key and the actual key
        // if identical than the variable is already declared
        if(scope->found(result)){
            // The variable has already been declared in the current scope
            throw std::runtime_error("Variable with identifier " + v.identifier + " declared on line "
                                     + std::to_string(v.lineNumber) + " already declared on line "
                                     + std::to_string(result->second.lineNumber));
        }
        // Go check the expression node
        // This will change the current type
        declarationNode->exprNode->accept(this);
        // Check current type with the declaration type
        // since the language does not perform any implicit/automatic typecast (as said in spec)
        if(declarationNode->type == currentType){
            scope->insert(v);
        }else{
            // throw an error since type casting is not supported
            throw std::runtime_error("Variable " + v.identifier + " was declared of type " + v.type + " on line "
                                     + std::to_string(v.lineNumber) + " but has been assigned invalid value of type"
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
        // Generate Variable
        Variable v(currentType, assignmentNode->identifier->identifier, assignmentNode->lineNumber);
        // Now confirm this exists in the function table for any scope
        for(const auto& scope : scopes){
            auto result = scope->find(v);
            if(scope->found(result)){
                // if identifier has been found
                // check that the types match
                if(result->second.type != v.type){
                    // throw an error since type casting is not supported
                    throw std::runtime_error("Variable " + v.identifier + " declared on line " + std::to_string(result->second.lineNumber)
                                                + " cannot be assigned a value of type " + v.type
                                                + ".\nImplicit and Automatic Typecasting is not supported by TeaLang.");
                }
                return;
            }
        }
        // Variable hasn't been found in any scope
        throw std::runtime_error("Variable with identifier " + v.identifier + " called on line "
                                 + std::to_string(v.lineNumber) + " has not been declared.");
    }

    void SemanticAnalyser::visit(parser::ASTPrintNode *printNode) {
        // Get the exprNode type
        printNode -> exprNode -> accept(this);
    }

    void SemanticAnalyser::visit(parser::ASTBlockNode *blockNode) {
        // Create new scope
        scopes.emplace_back(std::make_shared<Scope>());
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
        scopes.emplace_back(std::make_shared<Scope>());
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
        if (!scope->isGlobal()){
            throw std::runtime_error("Tried declaring function with identifier " + functionDeclarationNode->identifier->identifier
                                     + " in a non-global scope.");
        }
        // Create new scope for function params
        // This allows the creation of a new variables when they are params
        scopes.emplace_back(std::make_shared<Scope>());
        // Generate Function
        // First get the param types vector
        std::vector<std::string> paramTypes;
        for (const auto& param : functionDeclarationNode->parameters){
            paramTypes.emplace_back(param.second);
            // While going over the types add these to the new scope
            scopes.back()->insert(Variable(param.second, param.first, functionDeclarationNode->lineNumber));
        }
        // NOTE: The scope variable is still viewing the global scope
        // now generate the function object
        Function f(functionDeclarationNode->type, functionDeclarationNode->identifier->identifier, paramTypes, functionDeclarationNode->lineNumber);
        // Try to insert f
        auto result = scope->find(f);
        // compare the found key and the actual key
        // if identical than the function is already declared
        // Overloading is a problem for task 2 so we do not care if the params area actually different
        if(scope->found(result)){
            // The variable has already been declared in the current scope
            // Overloading is a problem for task 2
            throw std::runtime_error("Function with identifier " + f.identifier + " declared on line "
                                     + std::to_string(f.lineNumber) + " already declared on line "
                                     + std::to_string(result->second.lineNumber));
        }
        // insert function to the function table, this allows for recursion to happen
        scope->insert(f);
        // Go check the block node
        returns = false;
        functionDeclarationNode->functionBlock->accept(this);
        // confirm function has a return and that the return type is as defined in the declaration node
        if(!returns){
            throw std::runtime_error("Function with identifier " + f.identifier + " declared on line "
                                     + std::to_string(f.lineNumber) + " does not have a return statement.");
        }
        // Check that the return type matches with the function type
        if(functionDeclarationNode->type != currentType) {
            // Check current type with the declaration type
            // since the language does not perform any implicit/automatic typecast (as said in spec)
            throw std::runtime_error("Function " + f.identifier + " was declared of type " + f.type + " on line "
                                     + std::to_string(f.lineNumber) + " but has been assigned invalid value of type"
                                     + currentType +
                                     ".\nImplicit and Automatic Typecasting is not supported by TeaLang.");
        }
        // Close function scope
        // This discards any declared variable in the foo() section
        scopes.pop_back();
    }

    void SemanticAnalyser::visit(parser::ASTReturnNode *returnNode) {
        // Ensure returns is false
        if(returns){
            throw ReturnsException();
        }
        // Update current expression and update currentType
        returnNode -> exprNode -> accept(this);
        // mark returns as true
        returns = true;
    }
    // Statements
}