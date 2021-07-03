//
// Created by aiden on 04/05/2021.
//

#ifndef TEALANG_COMPILER_CPP20_VISITOR_H
#define TEALANG_COMPILER_CPP20_VISITOR_H

#include <string>

namespace parser {

    class ASTProgramNode;

    template <typename T> class ASTLiteralNode;
    class ASTBinaryNode;
    class ASTFunctionCallNode;
    class ASTIdentifierNode;
    class ASTUnaryNode;

    class ASTSFunctionCallNode;
    class ASTDeclarationNode;
    class ASTAssignmentNode;
    class ASTPrintNode;
    class ASTBlockNode;
    class ASTIfNode;
    class ASTForNode;
    class ASTWhileNode;
    class ASTFunctionDeclarationNode;
    class ASTReturnNode;
}

namespace visitor {

    class Visitor {

    public:

        virtual void visit(parser::ASTProgramNode*) = 0;

        virtual void visit(parser::ASTLiteralNode<int>*) = 0;
        virtual void visit(parser::ASTLiteralNode<float>*) = 0;
        virtual void visit(parser::ASTLiteralNode<bool>*) = 0;
        virtual void visit(parser::ASTLiteralNode<std::string>*) = 0;
        virtual void visit(parser::ASTBinaryNode*) = 0;
        virtual void visit(parser::ASTFunctionCallNode*) = 0;
        virtual void visit(parser::ASTIdentifierNode*) = 0;
        virtual void visit(parser::ASTUnaryNode*) = 0;

        virtual void visit(parser::ASTSFunctionCallNode*) = 0;
        virtual void visit(parser::ASTDeclarationNode*) = 0;
        virtual void visit(parser::ASTAssignmentNode*) = 0;
        virtual void visit(parser::ASTPrintNode*) = 0;
        virtual void visit(parser::ASTBlockNode*) = 0;
        virtual void visit(parser::ASTIfNode*) = 0;
        virtual void visit(parser::ASTForNode*) = 0;
        virtual void visit(parser::ASTWhileNode*) = 0;
        virtual void visit(parser::ASTFunctionDeclarationNode*) = 0;
        virtual void visit(parser::ASTReturnNode*) = 0;
    };

}

#endif //TEALANG_COMPILER_CPP20_VISITOR_H
