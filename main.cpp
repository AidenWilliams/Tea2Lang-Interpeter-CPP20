#include <iostream>
#include <memory>
#include "Lexer/Lexer.h"
#include "Lexer/Token.h"
#include "Parser/Parser.h"
#include "Parser/AST.h"
#include "Visitor/Visitor.h"
#include "Visitor/XML_Visitor.h"
#include "Visitor/Semantic_Visitor.h"
#include "Visitor/Interpreter_Visitor.h"

int main(int argc, char **argv) {
    if (std::string("-l") == argv[1]){
//        std::cout << "TESTING LEXER" << std::endl;
        lexer::Lexer lexer;
        lexer.extractLexemes(argv[2]);
        std::cout << "value: type" << std::endl;
        int j = 0;
        for (const auto& i: lexer.tokens){
            std::cout << j++ << " : " << i.value << " : " << i.type << std::endl;
        }
    }else if (std::string("-p") == argv[1]){
//        std::cout << "TESTING PARSER" << std::endl;
        lexer::Lexer lexer;
        lexer.extractLexemes(argv[2]);

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());
    }else if (std::string("-x") == argv[1]){
//        std::cout << "TESTING XML Generator" << std::endl;

        lexer::Lexer lexer;
        lexer.extractLexemes(argv[2]);

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());

        visitor::XMLVisitor xmlVisitor;
        auto *programNode1 = new parser::ASTProgramNode(programNode);
        xmlVisitor.visit(programNode1);

        delete programNode1;
    }else if (std::string("-s") == argv[1]){
//        std::cout << "TESTING Semantic Analyzer" << std::endl;

        lexer::Lexer lexer;
        lexer.extractLexemes(argv[2]); //_program_

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());

        visitor::SemanticAnalyser semanticAnalyser;
        auto *programNode1 = new parser::ASTProgramNode(programNode);
        semanticAnalyser.visit(programNode1);

        delete programNode1;
    }else if (std::string("-i") == argv[1]){
//        std::cout << "TESTING Interpreter" << std::endl;

        lexer::Lexer lexer;
        lexer.extractLexemes(argv[2]); //argv[2]

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());

        visitor::SemanticAnalyser semanticAnalyser;
        auto *programNode1 = new parser::ASTProgramNode(programNode);
        semanticAnalyser.visit(programNode1);

        visitor::Interpreter interpreter;
        interpreter.visit(programNode1);

        delete programNode1;
    }
    return 0;
}