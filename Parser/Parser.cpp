//
// Created by Aiden Williams on 03/05/2021.
//
#include <iostream>
#include "Parser.h"

//TODO: ADD INCORRECT UNIT TESTING (JUMBLE THE INPUT STRINGS)
namespace parser {
    void Parser::moveTokenWindow(int step) {
        // Move window by step
        currentToken = *(nextLoc + step - 1);
        nextLoc += step;
    }

    std::shared_ptr<ASTProgramNode> Parser::parseProgram(bool block) {
        auto statements = std::vector<std::shared_ptr<ASTStatementNode>>();
        // Loop over each token and stop with an END token
        while (currentToken.type != lexer::TOK_END && !(block && currentToken.type == lexer::TOK_CLOSING_CURLY)) {
            // Ignore comments and skip '}' if parsing a block
            if (currentToken.type != lexer::TOK_SINGLE_LINE_COMMENT
                && currentToken.type != lexer::TOK_MULTI_LINE_COMMENT
                && (!block || currentToken.type != lexer::TOK_CLOSING_CURLY))
                statements.push_back(parseStatement());
            // Get next Token
            // There is a case when a scope/block is empty where were need to check before moving the token window
            if (currentToken.type != lexer::TOK_END || nextLoc[0].type != lexer::TOK_END)
                moveTokenWindow();
        }
        return std::make_shared<ASTProgramNode>(statements);
    }

    std::shared_ptr<ASTExprNode> Parser::parseExpression() {
        auto simple_expr = std::shared_ptr<ASTExprNode>(parseSimpleExpression());
        unsigned int lineNumber = currentToken.lineNumber;
        std::string op;
        // Check if the next token is a relational operator
        if (nextLoc[0].type == lexer::TOK_LESS_THAN || nextLoc[0].type == lexer::TOK_MORE_THAN ||
            nextLoc[0].type == lexer::TOK_EQAUL_TO || nextLoc[0].type == lexer::TOK_NOT_EQAUL_TO ||
            nextLoc[0].type == lexer::TOK_LESS_THAN_EQUAL_TO || nextLoc[0].type == lexer::TOK_MORE_THAN_EQUAL_TO) {
            // store the operator
            op = nextLoc[0].value;
            // Move over current expression and operator (making the right side expression the current token)
            moveTokenWindow(2);
            // Parse right side expression and return
            return std::make_shared<ASTBinaryNode>(op, simple_expr, parseExpression(), lineNumber);
        }
        return simple_expr;
    }

    std::shared_ptr<ASTExprNode> Parser::parseSimpleExpression() {
        auto term = std::shared_ptr<ASTExprNode>(parseTerm());
        unsigned int lineNumber = currentToken.lineNumber;
        std::string op;
        // Check if the next token is an addition operator
        if (nextLoc[0].type == lexer::TOK_PLUS || nextLoc[0].type == lexer::TOK_MINUS ||
            nextLoc[0].type == lexer::TOK_OR) {
            // store the operator
            op = nextLoc[0].value;
            // Move over current simple expression and operator (making the right side simple expression the current token)
            moveTokenWindow(2);
            // Parse right side simple expression and return
            return std::make_shared<ASTBinaryNode>(op, term, parseSimpleExpression(), lineNumber);
        }
        return term;
    }

    std::shared_ptr<ASTExprNode> Parser::parseTerm() {
        auto factor = std::shared_ptr<ASTExprNode>(parseFactor());
        unsigned int lineNumber = currentToken.lineNumber;
        std::string op;
        // Check if the next token is an multiplication operator
        if (nextLoc[0].type == lexer::TOK_ASTERISK || nextLoc[0].type == lexer::TOK_DIVIDE ||
            nextLoc[0].type == lexer::TOK_AND) {
            // store the operator
            op = nextLoc[0].value;
            // Move over current term and operator (making the right side term the current token)
            moveTokenWindow(2);
            // Parse right side term and return
            return std::make_shared<ASTBinaryNode>(op, factor, parseTerm(), lineNumber);
        }
        return factor;
    }

    std::shared_ptr<ASTExprNode> Parser::parseFactor() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Define operator for Unary
        std::string op;
        // check current token type
        switch (currentToken.type) {
            // Literal Cases
            case lexer::TOK_INT:
                return std::make_shared<ASTLiteralNode<int>>(std::stoi(currentToken.value), lineNumber);
            case lexer::TOK_FLOAT:
                return std::make_shared<ASTLiteralNode<float>>(std::stof(currentToken.value), lineNumber);
            case lexer::TOK_TRUE:
                return std::make_shared<ASTLiteralNode<bool>>(true, lineNumber);
            case lexer::TOK_FALSE:
                return std::make_shared<ASTLiteralNode<bool>>(false, lineNumber);
            case lexer::TOK_STRING: {
                // Remove " character from front and end of lexeme
                std::string str = currentToken.value.substr(1, currentToken.value.size() - 2);
                // Replace \" with quote
                size_t pos = str.find("\\\"");
                while (pos != std::string::npos) {
                    // Replace
                    str.replace(pos, 2, "\"");
                    // Get next occurrence from current position
                    pos = str.find("\\\"", pos + 2);
                }
                // Replace \n with newline
                pos = str.find("\\n");
                while (pos != std::string::npos) {
                    // Replace
                    str.replace(pos, 2, "\n");
                    // Get next occurrence from current position
                    pos = str.find("\\n", pos + 2);
                }
                // Replace \t with tab
                pos = str.find("\\t");
                while (pos != std::string::npos) {
                    // Replace
                    str.replace(pos, 2, "\t");
                    // Get next occurrence from current position
                    pos = str.find("\\t", pos + 2);
                }
                // Replace \b with backslash
                pos = str.find("\\b");
                while (pos != std::string::npos) {
                    // Replace
                    str.replace(pos, 2, "\\");
                    // Get next occurrence from current position
                    pos = str.find("\\b", pos + 2);
                }
                return std::make_shared<ASTLiteralNode<std::string>>(std::move(str), lineNumber);
            }
                // Identifier or Function call case
            case lexer::TOK_IDENTIFIER:
                // If next token is '(' then we found a Function call
                if (nextLoc->type == lexer::TOK_OPENING_CURVY)
                    return parseFunctionCall();
                else {
                    // if not, its just an identifier
                    return std::make_shared<ASTIdentifierNode>(currentToken.value, lineNumber);
                }
                // Subexpression case
            case lexer::TOK_OPENING_CURVY:
                return parseSubExpression();
                // Unary expression case
            case lexer::TOK_MINUS:
            case lexer::TOK_NOT:
                // Current token is either not or -
                // store the operator
                op = currentToken.value;
                // Move over it
                moveTokenWindow();
                // return an ASTUnaryNode
                return std::make_shared<ASTUnaryNode>(parseExpression(), op, currentToken.lineNumber);
            default:
                throw std::runtime_error("Expected expression on line "
                                         + std::to_string(currentToken.lineNumber) + ".");
        }
    }

    std::vector<std::shared_ptr<ASTExprNode>> Parser::parseActualParams() {
        auto parameters = std::vector<std::shared_ptr<ASTExprNode>>();
        // Add first param
        parameters.emplace_back(parseExpression());
        // If next token is a comma there are more
        while (nextLoc->type == lexer::TOK_COMMA) {
            // Move current token, to token after comma
            moveTokenWindow(2);
            // Add this token
            parameters.emplace_back(parseExpression());
        }
        // Current token is on the last param, we need to move beyond that to get the closing )
        moveTokenWindow();
        return parameters;
    }

    std::shared_ptr<ASTFunctionCallNode> Parser::parseFunctionCall(bool semicolon) {
        auto parameters = std::vector<std::shared_ptr<ASTExprNode>>();
        unsigned int lineNumber = currentToken.lineNumber;
        // current token is the Function identifier
        auto identifier = std::make_shared<ASTIdentifierNode>(currentToken.value, lineNumber);
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax
        if (currentToken.type != lexer::TOK_OPENING_CURVY)
            throw std::runtime_error("Expected '(' on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // If next token is not right bracket, we have parameters
        if (currentToken.type != lexer::TOK_CLOSING_CURVY) {
            parameters = parseActualParams();
        }
        // Ensure right close bracket after fetching parameters
        if (currentToken.type != lexer::TOK_CLOSING_CURVY)
            throw std::runtime_error("Expected ')' on line "
                                     + std::to_string(currentToken.lineNumber)
                                     + " after Function parameters.");
        if (semicolon) {
            // Get next token
            moveTokenWindow();
            if (currentToken.type != lexer::TOK_SEMICOLON)
                throw std::runtime_error("Expected ';' after ')' on line "
                                         + std::to_string(currentToken.lineNumber) + ".");
        }
        return std::make_shared<ASTFunctionCallNode>(identifier, parameters, lineNumber);
    }

    std::shared_ptr<ASTExprNode> Parser::parseSubExpression() {
        // current token is the curvy bracket
        // move over first curvy bracket
        moveTokenWindow();
        // Now we should be able to get an expression
        auto exprNode = parseExpression();
        // move over expression
        moveTokenWindow();
        // Ensure ')' is there
        if (currentToken.type != lexer::TOK_CLOSING_CURVY)
            throw std::runtime_error("Expected ')' on line "
                                     + std::to_string(currentToken.lineNumber)
                                     + " after expression.");
        return exprNode;
    }

    std::string Parser::parseType() const {
        switch (currentToken.type) {
            case lexer::TOK_INT_TYPE:
            case lexer::TOK_FLOAT_TYPE:
            case lexer::TOK_BOOL_TYPE:
            case lexer::TOK_STRING_TYPE:
                return currentToken.value;
            default:
                throw std::runtime_error("Expected type after ':' on line "
                                         + std::to_string(currentToken.lineNumber) + ".");
        }
    }

    std::shared_ptr<ASTStatementNode> Parser::parseStatement() {
        // Parse a singular statement
        // The current token type determines what can be parsed
        switch (currentToken.type) {
            // Variable declaration case
            case lexer::TOK_LET:
                return parseDeclaration();
                // An identifier can either be a Function call or an assignment
            case lexer::TOK_IDENTIFIER:
                // If next token is '(' then we found a Function call
                if (nextLoc->type == lexer::TOK_OPENING_CURVY)
                    return std::make_shared<ASTSFunctionCallNode>(parseFunctionCall(true));
                else {
                    // if not, its should be an Assignment
                    return parseAssignment();
                }
                // Print case
            case lexer::TOK_PRINT:
                return parsePrint();
                // If case
            case lexer::TOK_IF:
                return parseIf();
                // For case
            case lexer::TOK_FOR:
                return parseFor();
                // While case
            case lexer::TOK_WHILE:
                return parseWhile();
                // Return case
            case lexer::TOK_RETURN:
                return parseReturn();
                // Function declaration cases
            case lexer::TOK_INT_TYPE:
            case lexer::TOK_FLOAT_TYPE:
            case lexer::TOK_BOOL_TYPE:
            case lexer::TOK_STRING_TYPE:
                return parseFunctionDeclaration();
                // Block parsing case
            case lexer::TOK_OPENING_CURLY:
                return parseBlock();
                // Any other case is an error case
            default:
                throw std::runtime_error("Invalid statement starting with '" +
                                         currentToken.value
                                         + "' encountered on line " +
                                         std::to_string(currentToken.lineNumber) + ".");
        }
    }

    std::shared_ptr<ASTDeclarationNode> Parser::parseDeclaration() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Current token is LET
        // Get next token
        moveTokenWindow();
        // Confirm current token is IDENTIFIER
        if (currentToken.type != lexer::TOK_IDENTIFIER)
            throw std::runtime_error("Expected Variable name after 'let' on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get identifier for new Variable
        auto identifier = std::make_shared<ASTIdentifierNode>(currentToken.value, lineNumber);
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax
        if (currentToken.type != lexer::TOK_COLON)
            throw std::runtime_error("Expected ':' after " + identifier->identifier + " on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Get type
        std::string type = parseType();
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax
        if (currentToken.type != lexer::TOK_EQUALS)
            throw std::runtime_error("Expected assignment operator '=' for " + identifier->identifier + " on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Get expression after =
        auto expr = parseExpression();
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax
        if (currentToken.type != lexer::TOK_SEMICOLON)
            throw std::runtime_error("Expected ';' after assignment of " + identifier->identifier + " on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Create ASTExpressionNode to return
        return std::make_shared<ASTDeclarationNode>(type, identifier, expr, lineNumber);
    }

    std::shared_ptr<ASTAssignmentNode> Parser::parseAssignment(bool _for) {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Current token is an IDENTIFIER
        auto identifier = std::make_shared<ASTIdentifierNode>(currentToken.value, lineNumber);
        // Get next token
        moveTokenWindow();
        // Token must be =
        // Ensure proper syntax
        if (currentToken.type != lexer::TOK_EQUALS)
            throw std::runtime_error("Expected assignment operator '=' for " + identifier->identifier + " on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Get expression after =
        auto expr = parseExpression();
        // Get next token
        moveTokenWindow();
        // Ensure proper; syntax
        if (!_for && currentToken.type != lexer::TOK_SEMICOLON)
            throw std::runtime_error("Expected ';' after assignment of " + identifier->identifier + " on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Create ASTAssignmentNode to return
        return std::make_shared<ASTAssignmentNode>(identifier, expr, lineNumber);
    }

    std::shared_ptr<ASTPrintNode> Parser::parsePrint() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Current token is PRINT
        // Get next token
        moveTokenWindow();
        // Get expression after print
        auto expr = parseExpression();
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax
        if (currentToken.type != lexer::TOK_SEMICOLON)
            throw std::runtime_error("Expected ';' after print on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Create ASTPrintNode to return
        return std::make_shared<ASTPrintNode>(expr, lineNumber);
    }

    std::shared_ptr<ASTBlockNode> Parser::parseBlock() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Current token is {
        // Get next token
        moveTokenWindow();
        // By definition a block is a program enclosed in { }
        auto block = std::make_shared<ASTBlockNode>(parseProgram(true), lineNumber);
        // closing } is handled by parseProgram
        // Create ASTBlockNode to return
        return std::make_shared<ASTBlockNode>(block->statements, lineNumber);
    }

    std::shared_ptr<ASTIfNode> Parser::parseIf() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Current token is IF
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with starting (
        if (currentToken.type != lexer::TOK_OPENING_CURVY)
            throw std::runtime_error("Expected '(' after if on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Get condition after (
        auto condition = parseExpression();
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with closing )
        if (currentToken.type != lexer::TOK_CLOSING_CURVY)
            throw std::runtime_error("Expected ')' after condition on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with starting {
        if (currentToken.type != lexer::TOK_OPENING_CURLY)
            throw std::runtime_error("Expected '{' after ')' on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // get if block
        auto ifBlock = parseBlock();
        // Check for ELSE
        auto elseBlock = std::shared_ptr<ASTBlockNode>();
        if (nextLoc[0].type == lexer::TOK_ELSE) {
            // Get next token
            moveTokenWindow(2);
            // Ensure proper syntax with starting {
            if (currentToken.type != lexer::TOK_OPENING_CURLY)
                throw std::runtime_error("Expected '{' after else on line "
                                         + std::to_string(currentToken.lineNumber) + ".");
            // get else block
            elseBlock = parseBlock();
        }
        // Create ASTIfNode to return
        return std::make_shared<ASTIfNode>(condition, ifBlock, lineNumber, elseBlock);
    }

    std::shared_ptr<ASTForNode> Parser::parseFor() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Current token is FOR
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with starting (
        if (currentToken.type != lexer::TOK_OPENING_CURVY)
            throw std::runtime_error("Expected '(' after for on line "
                                     + std::to_string(lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Check for declaration
        auto declaration = std::shared_ptr<ASTDeclarationNode>();
        if (currentToken.type == lexer::TOK_LET) {
            // get declaration
            declaration = parseDeclaration();
        } else {
            if (currentToken.type != lexer::TOK_SEMICOLON)
                throw std::runtime_error("Expected ';' after 'for( ;' on line "
                                         + std::to_string(currentToken.lineNumber) + ".");
        }
        // Get next token
        moveTokenWindow();
        // get condition
        auto condition = parseExpression();
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax
        if (currentToken.type != lexer::TOK_SEMICOLON)
            throw std::runtime_error("Expected ';' after condition on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Check for assignment
        auto assignment = std::shared_ptr<ASTAssignmentNode>();
        if (currentToken.type == lexer::TOK_IDENTIFIER) {
            // get declaration
            assignment = parseAssignment(true);
        }

        // Ensure proper syntax with closing )
        if (currentToken.type != lexer::TOK_CLOSING_CURVY)
            throw std::runtime_error("Expected ')' after condition on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with starting {
        if (currentToken.type != lexer::TOK_OPENING_CURLY)
            throw std::runtime_error("Expected '{' after ')' on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // get if block
        auto loopBlock = parseBlock();

        // Create ASTForNode to return
        return std::make_shared<ASTForNode>(condition, loopBlock, lineNumber, declaration, assignment);
    }

    std::shared_ptr<ASTWhileNode> Parser::parseWhile() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Current token is WHILE
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with starting (
        if (currentToken.type != lexer::TOK_OPENING_CURVY)
            throw std::runtime_error("Expected '(' after while on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Get condition after (
        auto condition = parseExpression();
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with closing )
        if (currentToken.type != lexer::TOK_CLOSING_CURVY)
            throw std::runtime_error("Expected ')' after condition on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with starting {
        if (currentToken.type != lexer::TOK_OPENING_CURLY)
            throw std::runtime_error("Expected '{' after ')' on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // get loop Block
        auto loopBlock = parseBlock();
        // Create ASTWhileNode to return
        return std::make_shared<ASTWhileNode>(condition, loopBlock, lineNumber);
    }

    std::shared_ptr<ASTReturnNode> Parser::parseReturn() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Current token is RETURN
        // Get next token
        moveTokenWindow();
        // Get expression after
        auto expr = parseExpression();
        // Get next token
        moveTokenWindow();

        if (currentToken.type != lexer::TOK_SEMICOLON)
            throw std::runtime_error("Expected ';' after expression on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Create ASTReturnNode to return
        return std::make_shared<ASTReturnNode>(expr, lineNumber);
    }

    std::vector <std::pair<std::string, std::string>> Parser::parseFormalParams() {
        //current token is identifier
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        auto parameters = std::vector <std::pair<std::string, std::string>>();
        // get first identifier
        // ensure identifier is here

        auto identifier = std::shared_ptr<ASTIdentifierNode>();
        if (currentToken.type == lexer::TOK_IDENTIFIER) {
            identifier = std::make_shared<ASTIdentifierNode>(currentToken.value, lineNumber);
        } else {
            throw std::runtime_error("Expected Function name after type on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        }
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with : after identifier
        if (currentToken.type != lexer::TOK_COLON)
            throw std::runtime_error("Expected ':' after " + identifier->identifier + " on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // Get next token
        moveTokenWindow();
        // get first type
        std::string type = parseType();
        // Add first param
        parameters.emplace_back(std::pair < std::string, std::string > {identifier->identifier, type});
        // If next token is a comma there are more
        while (nextLoc->type == lexer::TOK_COMMA) {
            // Move current token, to token after comma
            moveTokenWindow(2);
            // repeat the above steps
            if (currentToken.type == lexer::TOK_IDENTIFIER) {
                identifier = std::make_shared<ASTIdentifierNode>(currentToken.value, lineNumber);
            } else {
                throw std::runtime_error("Expected Function name after type on line "
                                         + std::to_string(currentToken.lineNumber) + ".");
            }
            // Get next token
            moveTokenWindow();
            // Ensure proper syntax with : after identifier
            if (currentToken.type != lexer::TOK_COLON)
                throw std::runtime_error("Expected ':' after " + identifier->identifier + " on line "
                                         + std::to_string(currentToken.lineNumber) + ".");
            // Get next token
            moveTokenWindow();
            // get  type
            type = parseType();
            // Add first param
            parameters.emplace_back(std::pair < std::string, std::string > {identifier->identifier, type});
        }
        // Current token is on the last param, we need to move beyond that to get the closing )
        moveTokenWindow();
        return parameters;
    }

    std::shared_ptr<ASTFunctionDeclarationNode> Parser::parseFunctionDeclaration() {
        // Determine line number
        unsigned int lineNumber = currentToken.lineNumber;
        // Get type
        std::string type = parseType();
        // Get next token
        moveTokenWindow();
        // ensure identifier is here
        auto identifier = std::shared_ptr<ASTIdentifierNode>();
        if (currentToken.type == lexer::TOK_IDENTIFIER) {
            identifier = std::make_shared<ASTIdentifierNode>(currentToken.value, lineNumber);
        } else {
            throw std::runtime_error("Expected Function name after type on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        }
        // Get next token
        moveTokenWindow(2);
        // If next token is not right bracket, we have parameters
        auto parameters = std::vector <std::pair<std::string, std::string>>();
        if (currentToken.type != lexer::TOK_CLOSING_CURVY) {
            parameters = parseFormalParams();
        }
        // Ensure right close bracket after fetching parameters
        if (currentToken.type != lexer::TOK_CLOSING_CURVY)
            throw std::runtime_error("Expected ')' on line "
                                     + std::to_string(currentToken.lineNumber)
                                     + " after Function parameters.");
        // Get next token
        moveTokenWindow();
        // Ensure proper syntax with starting {
        if (currentToken.type != lexer::TOK_OPENING_CURLY)
            throw std::runtime_error("Expected '{' after ')' on line "
                                     + std::to_string(currentToken.lineNumber) + ".");
        // get loop Block
        auto functionBlock = parseBlock();
        // Create ASTFunctionDeclarationNode to return
        return std::make_shared<ASTFunctionDeclarationNode>(type, identifier, parameters, functionBlock, lineNumber);
    }
}