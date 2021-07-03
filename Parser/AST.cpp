//
// Created by Aiden Williams on 26/06/2021.
//

#include "AST.h"


// Accept functions for visitors

//  Program

namespace parser {
    void ASTProgramNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

//  Program

// Expression

    template<>
    void ASTLiteralNode<int>::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    template<>
    void ASTLiteralNode<float>::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    template<>
    void ASTLiteralNode<bool>::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    template<>
    void ASTLiteralNode<std::string>::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTBinaryNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTFunctionCallNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTIdentifierNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTUnaryNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

// Expression

// Statement

    void ASTSFunctionCallNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTDeclarationNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTAssignmentNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTPrintNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTBlockNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTIfNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTForNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTWhileNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTFunctionDeclarationNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }

    void ASTReturnNode::accept(visitor::Visitor *v) {
        v->visit(this);
    }
}
// Statement
