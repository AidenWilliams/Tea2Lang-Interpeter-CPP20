//
// Created by Aiden Williams on 03/05/2021.
//

#include "Parser.h"


void parser::Parser::moveTokenWindow(int step){
    currentToken += step;
    nextToken += step;
}

parser::ASTProgramNode* parser::Parser::parseProgram() {

    auto statements = new std::vector<ASTStatementNode*>;

    while(currentToken->type != lexer::TOK_END){
        statements->push_back(parseStatement());
        moveTokenWindow();
    }

    return new ASTProgramNode(*statements);
}

parser::ASTStatementNode* parser::Parser::parseStatement() {
    switch(currentToken->type){
        case lexer::TOK_LET:
            return parseDeclaration();
//        case lexer::TOK_SET:
//            return parse_assignment_statement();
//
//        case lexer::TOK_PRINT:
//            return parse_print_statement();
//
//        case lexer::TOK_IF:
//            return parse_if_statement();
//
//        case lexer::TOK_WHILE:
//            return parse_while_statement();
//
//        case lexer::TOK_RETURN:
//            return parse_return_statement();
//
//        case lexer::TOK_DEF:
//            return parse_function_definition();
//
//        case lexer::TOK_LEFT_CURLY:
//            return parse_block();

        default:
            throw std::runtime_error("Invalid statement starting with '" +
                                     currentToken->value
                                     + "' encountered on line " +
                                     std::to_string(currentToken->lineNumber) + ".");
    }
}

parser::ASTDeclarationNode* parser::Parser::parseDeclaration() {
    // Node attributes
    TYPE type;
    std::string identifier;
    ASTExprNode* expr;
    unsigned int line_number;

    // Determine line number
    line_number = currentToken->lineNumber;
    // Get next token
    moveTokenWindow();
    // Get identifier for new variable

    if(currentToken->type != lexer::TOK_IDENTIFIER)
        throw std::runtime_error("Expected variable name after 'let' on line "
                                 + std::to_string(currentToken->lineNumber) + ".");
    identifier = currentToken->value;

    // Get next token
    moveTokenWindow();

    // Ensure proper syntax
    if(currentToken->type != lexer::TOK_COLON)
        throw std::runtime_error("Expected ':' after " + identifier + " on line "
                                 + std::to_string(currentToken->lineNumber) + ".");

    // Get next token
    moveTokenWindow();
    // Get type
    try{
        type = parseType(identifier);
    }catch (error_t T){
        throw std::runtime_error("Expected type for " + identifier + " after ':' on line "
                                 + std::to_string(currentToken->lineNumber) + ".");
    }

    // Get next token
    moveTokenWindow();
    // Ensure proper syntax
    if(currentToken->type != lexer::TOK_EQUALS)
        throw std::runtime_error("Expected assignment operator '=' for " + identifier + " on line "
                                 + std::to_string(currentToken->lineNumber) + ".");

    // Get expression after =
    expr = parseExpression();

    // Get next token
    moveTokenWindow();
    // Ensure proper syntax
    if(currentToken->type != lexer::TOK_SEMICOLON)
        throw std::runtime_error("Expected ';' after assignment of " + identifier + " on line "
                                 + std::to_string(currentToken->lineNumber) + ".");

    // Create ASTExpressionNode to return
    return new ASTDeclarationNode(type, identifier, expr, line_number);
}


parser::TYPE parser::Parser::parseType(const std::string& identifier) {
    switch(currentToken->type){
        case lexer::TOK_INT_TYPE:
            return INT;

        case lexer::TOK_FLOAT_TYPE:
            return FLOAT;

        case lexer::TOK_BOOL_TYPE:
            return BOOL;

        case lexer ::TOK_STRING_TYPE:
            return STRING;

        default:
            throw std::runtime_error("Expected type for " + identifier + " on line "
                                     + std::to_string(currentToken->lineNumber) + ".");
    }
}