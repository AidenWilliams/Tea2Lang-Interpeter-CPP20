//
// Created by Aiden Williams on 03/05/2021.
//

#ifndef TEALANG_COMPILER_CPP20_PARSER_H
#define TEALANG_COMPILER_CPP20_PARSER_H

#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include "../Visitor/Visitor.h"
#include "../Lexer/Lexer.h"
#include "../Lexer/Token.h"
#include "AST.h"

namespace parser {
    class Parser {
    public:
        explicit Parser(std::vector<lexer::Token> tokens){
            // Initialise the currentToken and pointer for the next token
            currentToken = tokens.front();
            nextLoc = tokens.begin() + 1;
        }

        std::shared_ptr<ASTProgramNode> parseProgram(bool block=false);
        std::shared_ptr<ASTStatementNode> parseStatement();
        std::shared_ptr<ASTDeclarationNode> parseDeclaration();
        std::shared_ptr<ASTAssignmentNode> parseAssignment(bool _for=false);
        std::shared_ptr<ASTPrintNode> parsePrint();
        std::shared_ptr<ASTBlockNode> parseBlock();
        std::shared_ptr<ASTIfNode> parseIf();
        std::shared_ptr<ASTForNode> parseFor();
        std::shared_ptr<ASTWhileNode> parseWhile();
        std::shared_ptr<ASTReturnNode> parseReturn();
        std::shared_ptr<ASTFunctionDeclarationNode> parseFunctionDeclaration();
        std::vector<std::pair<std::string, std::string>> parseFormalParams();
        std::shared_ptr<ASTExprNode> parseExpression();
        std::shared_ptr<ASTExprNode> parseSimpleExpression();
        std::shared_ptr<ASTExprNode> parseTerm();
        std::shared_ptr<ASTExprNode> parseFactor();
        std::vector<std::shared_ptr<ASTExprNode>> parseActualParams();
        std::shared_ptr<ASTFunctionCallNode> parseFunctionCall(bool semicolon=false);
        std::shared_ptr<ASTExprNode> parseSubExpression();

        [[nodiscard]] std::string parseType() const;

    private:
        lexer::Token currentToken;
        std::vector<lexer::Token>::iterator nextLoc;

        void moveTokenWindow(int step = 1);
    };
}
#endif //TEALANG_COMPILER_CPP20_PARSER_H
