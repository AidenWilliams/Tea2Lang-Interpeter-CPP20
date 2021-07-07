#include <iostream>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Visitor/XML_Visitor.h"
#include "Visitor/Semantic_Visitor.h"
#include "Visitor/Interpreter_Visitor.h"

int main(int argc, char **argv) {

    std::string _program_;
    if(argv[2] == std::string("-p")){
        std::ifstream file(argv[3]);
        if (!file){
            std::cerr << "Unable to read file!" << std::endl;
            throw std::runtime_error("Unable to read file!");
        }
        std::stringstream ss;
        ss << file.rdbuf();
        _program_ = ss.str();
    }else{
        _program_ = std::string(argv[3]);
    }
    if (std::string("-l") == argv[1]){
//        std::cout << "TESTING LEXER" << std::endl;
        lexer::Lexer lexer;
        lexer.extractLexemes(_program_);
    }else if (std::string("-p") == argv[1]){
//        std::cout << "TESTING PARSER" << std::endl;
        lexer::Lexer lexer;
        lexer.extractLexemes(_program_);

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());
    }else if (std::string("-x") == argv[1]) {
//        std::cout << "TESTING XML Generator" << std::endl;

        lexer::Lexer lexer;
        lexer.extractLexemes(_program_);

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());

        visitor::XMLVisitor xmlVisitor;
        auto *programNode1 = new parser::ASTProgramNode(programNode);
        xmlVisitor.visit(programNode1);

        delete programNode1;
    }else if (std::string("-s") == argv[1]) {
//        std::cout << "TESTING Semantic Analyzer" << std::endl;
        lexer::Lexer lexer;
        lexer.extractLexemes(_program_); //_program_

        parser::Parser parser(lexer.tokens);
        auto programNode = std::shared_ptr<parser::ASTProgramNode>(parser.parseProgram());

        visitor::SemanticAnalyser semanticAnalyser;
        auto *programNode1 = new parser::ASTProgramNode(programNode);
        semanticAnalyser.visit(programNode1);

        delete programNode1;
    }else if (std::string("-i") == argv[1]){
//        std::cout << "TESTING Interpreter" <<  std::endl;

        lexer::Lexer lexer;
        lexer.extractLexemes(_program_); //argv[2]

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