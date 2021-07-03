#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by Aiden Williams on 29/04/2021.
//

#include "Token.h"

namespace lexer {
    std::regex identifier("^[a-zA-Z_]*[a-zA-Z0-9_]*$");
    std::regex string(R"(\"(\\.|[^"\\])*\")");
    std::regex intLiteral("^[0-9]*$");
    std::regex floatLiteral("^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$");
    std::regex singleLineComment(R"(^\/\/[^\n\r]+(?:[\n\r]|\*\))$)");
    std::regex multiLineComment(R"(\/\*(\*(?!\/)|[^*])*\*\/$)");

    bool isFloatType(const std::string &s) {
        return s == "float";
    }

    bool isIntType(const std::string &s) {
        return s == "int";
    }

    bool isBoolType(const std::string &s) {
        return s == "bool";
    }

    bool isStringType(const std::string &s) {
        return s == "string";
    }

    bool isTrue(const std::string &s) {
        return s == "true";
    }

    bool isFalse(const std::string &s) {
        return s == "false";
    }

    bool isIdentifier(const std::string &s) {
        return (std::regex_match(s, identifier));
    }

    bool isAnd(const std::string &s) {
        return s == "and";
    }

    bool isOr(const std::string &s) {
        return s == "or";
    }

    bool isNot(const std::string &s) {
        return s == "not";
    }

    bool isLet(const std::string &s) {
        return s == "let";
    }

    bool isPrint(const std::string &s) {
        return s == "print";
    }

    bool isReturn(const std::string &s) {
        return s == "return";
    }

    bool isIf(const std::string &s) {
        return s == "if";
    }

    bool isElse(const std::string &s) {
        return s == "else";
    }

    bool isFor(const std::string &s) {
        return s == "for";
    }

    bool isWhile(const std::string &s) {
        return s == "while";
    }

    bool isString(const std::string &s) {
        return (std::regex_match(s, string));
    }

    bool isInt(const std::string &s) {
        return (std::regex_match(s, intLiteral));
    }

    bool isFloat(const std::string &s) {
        return (std::regex_match(s, floatLiteral));
    }

    bool isDivide(const std::string &s) {
        return s == "/";
    }

    bool isSingleLineComment(const std::string &s) {
        return (std::regex_match(s, singleLineComment));
    }

    bool isMultiLineComment(const std::string &s) {
        return (std::regex_match(s, multiLineComment));
    }

    bool isEnd(const std::string &s) {
        return s.empty();
    }

    bool isOpeningCurly(const std::string &s) {
        return s == "{";
    }

    bool isClosingCurly(const std::string &s) {
        return s == "}";
    }

    bool isOpeningCurvy(const std::string &s) {
        return s == "(";
    }

    bool isClosingCurvy(const std::string &s) {
        return s == ")";
    }

    bool isComma(const std::string &s) {
        return s == ",";
    }

    bool isColon(const std::string &s) {
        return s == ":";
    }

    bool isSemiColon(const std::string &s) {
        return s == ";";
    }

    bool isMinus(const std::string &s) {
        return s == "-";
    }

    bool isAsterisk(const std::string &s) {
        return s == "*";
    }

    bool isPlus(const std::string &s) {
        return s == "+";
    }

    bool isMoreThan(const std::string &s) {
        return s == ">";
    }

    bool isLessThan(const std::string &s) {
        return s == "<";
    }

    bool isEquals(const std::string &s) {
        return s == "=";
    }

    bool isMoreThanEqualTo(const std::string &s) {
        return s == ">=";
    }

    bool isLessThanEqualTo(const std::string &s) {
        return s == "<=";
    }

    bool isNotEqualTo(const std::string &s) {
        return s == "!=";
    }

    bool isEqualTo(const std::string &s) {
        return s == "==";
    }

    TOKEN_TYPE determineOperatorType(const std::string& op){
        // Multiplicative
        if(isAnd(op)) return TOK_AND;
        if (isAsterisk(op)) return TOK_ASTERISK;
        if (isDivide(op)) return TOK_DIVIDE;
        // Additive
        if (isPlus(op)) return TOK_PLUS;
        // Relational
        if (isMoreThan(op)) return TOK_MORE_THAN;
        if (isLessThan(op)) return TOK_LESS_THAN;
        if (isMoreThanEqualTo(op)) return TOK_MORE_THAN_EQUAL_TO;
        if (isLessThanEqualTo(op)) return TOK_LESS_THAN_EQUAL_TO;
        if (isNotEqualTo(op)) return TOK_NOT_EQAUL_TO;
        if (isEqualTo(op)) return TOK_EQAUL_TO;
        // Unary
        if (isNot(op)) return TOK_NOT;
        if (isMinus(op)) return TOK_MINUS;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState1(const std::string &s) {
        // Keywords
        if (isFloatType(s)) return TOK_FLOAT_TYPE;
        if (isIntType(s)) return TOK_INT_TYPE;
        if (isBoolType(s)) return TOK_BOOL_TYPE;
        if (isStringType(s)) return TOK_STRING_TYPE;
        if (isTrue(s)) return TOK_TRUE;
        if (isFalse(s)) return TOK_FALSE;
        if (isAnd(s)) return TOK_AND;
        if (isOr(s)) return TOK_OR;
        if (isNot(s)) return TOK_NOT;
        if (isLet(s)) return TOK_LET;
        if (isPrint(s)) return TOK_PRINT;
        if (isReturn(s)) return TOK_RETURN;
        if (isIf(s)) return TOK_IF;
        if (isElse(s)) return TOK_ELSE;
        if (isFor(s)) return TOK_FOR;
        if (isWhile(s)) return TOK_WHILE;
        // Identifier
        if (isIdentifier(s)) return TOK_IDENTIFIER;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState3(const std::string &s) {
        // String
        if (isString(s)) return TOK_STRING;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState6(const std::string &s) {
        // Integer
        if (isInt(s)) return TOK_INT;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState8(const std::string &s) {
        // Float
        if (isFloat(s)) return TOK_FLOAT;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState9(const std::string &s) {
        // Divide
        if (isDivide(s)) return TOK_DIVIDE;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState11(const std::string &s) {
        // Single Line Comment
        if (isSingleLineComment(s)) return TOK_SINGLE_LINE_COMMENT;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState14(const std::string &s) {
        // Multi Line Comment
        if (isMultiLineComment(s)) return TOK_MULTI_LINE_COMMENT;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState15(const std::string &s) {
        // EOF
        if (isEnd(s)) return TOK_END;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState16(const std::string &s) {
        // Punctuation
        if (isOpeningCurly(s)) return TOK_OPENING_CURLY;
        if (isClosingCurly(s)) return TOK_CLOSING_CURLY;
        if (isOpeningCurvy(s)) return TOK_OPENING_CURVY;
        if (isClosingCurvy(s)) return TOK_CLOSING_CURVY;
        if (isComma(s)) return TOK_COMMA;
        if (isColon(s)) return TOK_COLON;
        if (isSemiColon(s)) return TOK_SEMICOLON;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState17(const std::string &s) {
        // Minus
        if (isMinus(s)) return TOK_MINUS;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState18(const std::string &s) {
        // Asterisk
        if (isAsterisk(s)) return TOK_ASTERISK;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState19(const std::string &s) {
        // Plus
        if (isPlus(s)) return TOK_PLUS;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState21(const std::string &s) {
        // More Than
        if (isMoreThan(s)) return TOK_MORE_THAN;
        // Less Than
        if (isLessThan(s)) return TOK_LESS_THAN;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState22(const std::string &s) {
        // Equals
        if (isEquals(s)) return TOK_EQUALS;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState23(const std::string &s) {
        // More Than or Equal To
        if (isMoreThanEqualTo(s)) return TOK_MORE_THAN_EQUAL_TO;
        // Less Than or Equal To
        if (isLessThanEqualTo(s)) return TOK_LESS_THAN_EQUAL_TO;
        // Not Equal To
        if (isNotEqualTo(s)) return TOK_NOT_EQAUL_TO;
        // Equal To
        if (isEqualTo(s)) return TOK_EQAUL_TO;
        return TOK_INVALID;
    }

    TOKEN_TYPE fromState25(const std::string &s) {
        // Can only be Closing Curly
        if (isClosingCurly(s)) return TOK_CLOSING_CURLY;
        return TOK_INVALID;
    }

    TOKEN_TYPE Token::determineTokenType(std::string &s, unsigned int state) {
        switch (state) {
            case 0:
                return TOK_END;
            case 1:
                return fromState1(s);
            case 3:
                return fromState3(s);
            case 6:
                return fromState6(s);
            case 8:
                return fromState8(s);
            case 9:
                return fromState9(s);
            case 11:
                return fromState11(s);
            case 14:
                return fromState14(s);
            case 15:
                return fromState15(s);
            case 16:
                return fromState16(s);
            case 17:
                return fromState17(s);
            case 18:
                return fromState18(s);
            case 19:
                return fromState19(s);
            case 21:
                return fromState21(s);
            case 22:
                return fromState22(s);
            case 23:
                return fromState23(s);
            case 25:
                return fromState25(s);
            default:
                return TOK_INVALID;
        }
    }
}

#pragma clang diagnostic pop