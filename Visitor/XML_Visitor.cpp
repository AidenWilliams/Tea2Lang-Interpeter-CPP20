//
// Created by aiden on 25/06/2021.
//


#include "XML_Visitor.h"

// TODO: ADD UNITTESTER XML GENERATION

namespace visitor {
    std::string XMLVisitor::indentation() {
        std::string tabs;
        // Add a tab (4 spaces) according to the indentation level
        for (unsigned int i = 0; i < indentationLevel; i++)
            tabs += TAB;
        return tabs;
    }

    std::string XMLVisitor::xmlSafeOp(std::string op) {
        // changes unsafe characters into safe ones for XML parser
        if (op == "<")
            return "&lt;";
        if (op == ">")
            return "&gt;";
        if (op == "<=")
            return "&lt;=";
        if (op == ">=")
            return "&gt;=";
        return op;
    }

    void XMLVisitor::visit(parser::ASTProgramNode* programNode) {
        // Add initial <program> tag
        xmlfile << indentation() << "<program>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // For each statement, generate XML for it
        for (auto &statement : programNode->statements)
            statement->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</program>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTLiteralNode<int> *literalNode) {
        // Add initial <int> tag
        xmlfile << indentation() << "<int>";
        // Add value
        xmlfile << std::to_string(literalNode->val);
        // Add closing tag
        xmlfile << "</int>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTLiteralNode<float> *literalNode) {
        // Add initial <float> tag
        xmlfile << indentation() << "<float>";
        // Add value
        xmlfile << std::to_string(literalNode->val);
        // Add closing tag
        xmlfile << "</float>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTLiteralNode<bool> *literalNode) {
        // Add initial <bool> tag
        xmlfile << indentation() << "<bool>";
        // Add value
        xmlfile << ((literalNode->val) ? "true" : "false");
        // Add closing tag
        xmlfile << "</bool>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTLiteralNode<std::string> *literalNode) {
        // Add initial <string> tag
        xmlfile << indentation() << "<string>";
        // Add value
        xmlfile << literalNode->val;
        // Add closing tag
        xmlfile << "</string>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTBinaryNode *binaryNode) {
        // Add initial <bin> tag
        xmlfile << indentation() << "<bin op = \"" + xmlSafeOp(binaryNode->op) +
                                    "\">" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Left node
        binaryNode->left->accept(this);
        // Right node
        binaryNode->right->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</bin>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTIdentifierNode *identifierNode) {
        // Add initial <id> tag
        xmlfile << indentation() << "<id>";
        // Add value
        xmlfile << identifierNode->identifier;
        // Add closing tag
        xmlfile << "</id>" << std::endl;

    }

    void XMLVisitor::visit(parser::ASTUnaryNode *unaryExprNode) {
        // Add initial <unary> tag
        xmlfile << indentation() << "<unary op = \"" + unaryExprNode->op + "\">"
                << std::endl;
        // Add indentation level
        indentationLevel++;
        // Value
        unaryExprNode->exprNode->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</unary>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTFunctionCallNode *functionCallNode) {
        // Add initial <functionEcall> tag
        xmlfile << indentation() << "<functionEcall>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Function identifier
        xmlfile << indentation() << "<id>" + functionCallNode->identifier->identifier + "</id>" << std::endl;
        // For each parameter
        for (auto &param : functionCallNode->parameters) {
            xmlfile << indentation() << "<param>" << std::endl;
            // Add indentation level
            indentationLevel++;
            // Parameter
            param->accept(this);
            // Remove indentation level
            indentationLevel++;
            xmlfile << indentation() << "</param>" << std::endl;
        }
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</functionEcall>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTSFunctionCallNode *sFunctionCallNode) {
        // Add initial <functionScall> tag
        xmlfile << indentation() << "<functionScall>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Function identifier
        xmlfile << indentation() << "<id>" + sFunctionCallNode->identifier->identifier + "</id>" << std::endl;
        // For each parameter
        for (auto &param : sFunctionCallNode->parameters) {
            xmlfile << indentation() << "<param>" << std::endl;
            // Add indentation level
            indentationLevel++;
            // Parameter
            param->accept(this);
            // Remove indentation level
            indentationLevel++;
            xmlfile << indentation() << "</param>" << std::endl;
        }
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</functionScall>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTDeclarationNode *declarationNode) {
        // Add initial <declaration> tag
        xmlfile << indentation() << "<declaration>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Add identifier
        xmlfile << indentation() << "<id type = \"" + declarationNode->type + "\">"
                << declarationNode->identifier->identifier << "</id>" << std::endl;
        // Expression tags
        declarationNode->exprNode->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</declaration>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTAssignmentNode *assignmentNode) {
        // Add initial <assign> tag
        xmlfile << indentation() << "<assign>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Add identifier
        xmlfile << indentation() << "<id>" << assignmentNode->identifier->identifier << "</id>" << std::endl;
        // Expression tags
        assignmentNode->exprNode->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</assign>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTPrintNode *printNode) {
        // Add initial <print> tag
        xmlfile << indentation() << "<print>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Expression tags
        printNode->exprNode->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</print>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTBlockNode *blockNode) {
        // Add initial <block> tag
        xmlfile << indentation() << "<block>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // For each statement, accept
        for (auto &statement : blockNode->statements)
            statement->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</block>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTIfNode *ifNode) {
        // Add initial <if> tag
        xmlfile << indentation() << "<if>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Add <condition> tag
        xmlfile << indentation() << "<condition>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Expression
        ifNode->condition->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</condition>" << std::endl;
        // Add <ifBlock> tag
        xmlfile << indentation() << "<ifBlock>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // IfBlock
        ifNode->ifBlock->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</ifBlock>" << std::endl;
        // Remove indentation level
        indentationLevel--;
        // If there is an else-block
        if (ifNode->elseBlock) {
            // Add <else-block> tag
            xmlfile << indentation() << "<elseBlock>" << std::endl;
            // Add indentation level
            indentationLevel++;
            // elseBlock
            ifNode->elseBlock->accept(this);
            // Remove indentation level
            indentationLevel--;
            // Add closing tag
            xmlfile << indentation() << "</elseBlock>" << std::endl;
        }
        // Add closing tag
        xmlfile << indentation() << "</if>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTForNode *forNode) {
        // Add initial <while> tag
        xmlfile << indentation() << "<for>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Add <declaration> tag
        xmlfile << indentation() << "<declaration>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Expression
        forNode->declaration->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</declaration>" << std::endl;
        // Add <condition> tag
        xmlfile << indentation() << "<condition>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Expression
        forNode->condition->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</condition>" << std::endl;
        // Add <assignment> tag
        xmlfile << indentation() << "<assignment>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Expression
        forNode->assignment->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</assignment>" << std::endl;
        // loopBlock
        forNode->loopBlock->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</for>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTWhileNode *whileNode) {
        // Add initial <while> tag
        xmlfile << indentation() << "<while>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Add <condition> tag
        xmlfile << indentation() << "<condition>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Expression
        whileNode->condition->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</condition>" << std::endl;
        // loopBlock
        whileNode->loopBlock->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</while>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTFunctionDeclarationNode *functionDeclarationNode) {
        // Add initial <functionDeclaration> tag
        xmlfile << indentation() << "<functionDeclaration type = \"" + functionDeclarationNode->type +
                                    "\">" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Function identifier
        xmlfile << indentation() << "<id>" + functionDeclarationNode->identifier->identifier + "</id>" << std::endl;
        // For each parameter
        for (auto &param : functionDeclarationNode->parameters) {
            xmlfile << indentation() << "<param type = \"" + param.second +
                                        "\">" + param.first + "</param>" << std::endl;
        }
        // Function body
        functionDeclarationNode->functionBlock->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</functionDeclaration>" << std::endl;
    }

    void XMLVisitor::visit(parser::ASTReturnNode *returnNode) {
        // Add initial <return> tag
        xmlfile << indentation() << "<return>" << std::endl;
        // Add indentation level
        indentationLevel++;
        // Expression tags
        returnNode->exprNode->accept(this);
        // Remove indentation level
        indentationLevel--;
        // Add closing tag
        xmlfile << indentation() << "</return>" << std::endl;
    }
}