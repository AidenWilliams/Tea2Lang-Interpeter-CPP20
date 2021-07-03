//
// Created by aiden on 25/06/2021.
//

#ifndef TEALANG_COMPILER_CPP20_XML_VISITOR_H
#define TEALANG_COMPILER_CPP20_XML_VISITOR_H
#include <fstream>
#include <memory>
#include "Visitor.h"
#include "../Parser/AST.h"

namespace visitor{

    class XMLVisitor : public Visitor {

    public:
        XMLVisitor(): indentationLevel(0) {
            xmlfile.open("program.xml");
        }
        ~XMLVisitor(){
            xmlfile.close();
        }

        void visit(parser::ASTProgramNode* programNode) override;

        void visit(parser::ASTLiteralNode<int>* literalNode) override;
        void visit(parser::ASTLiteralNode<float>* literalNode) override;
        void visit(parser::ASTLiteralNode<bool>* literalNode) override;
        void visit(parser::ASTLiteralNode<std::string>* literalNode) override;
        void visit(parser::ASTBinaryNode* binaryNode) override;
        void visit(parser::ASTIdentifierNode* identifierNode) override;
        void visit(parser::ASTUnaryNode* unaryNode) override;
        void visit(parser::ASTFunctionCallNode* functionCallNode) override;

        void visit(parser::ASTSFunctionCallNode* sFunctionCallNode) override;
        void visit(parser::ASTDeclarationNode* declarationNode) override;
        void visit(parser::ASTAssignmentNode* assignmentNode) override;
        void visit(parser::ASTPrintNode* printNode) override;
        void visit(parser::ASTBlockNode* blockNode) override;
        void visit(parser::ASTIfNode* ifNode) override;
        void visit(parser::ASTForNode* forNode) override;
        void visit(parser::ASTWhileNode* whileNode) override;
        void visit(parser::ASTFunctionDeclarationNode* functionDeclarationNode) override;
        void visit(parser::ASTReturnNode* returnNode) override;

    private:
        std::ofstream xmlfile;
        unsigned int indentationLevel;
        const std::string TAB = "    ";
        std::string indentation();
        static std::string xmlSafeOp(std::string op);

    };
}
#endif //TEALANG_COMPILER_CPP20_XML_VISITOR_H
