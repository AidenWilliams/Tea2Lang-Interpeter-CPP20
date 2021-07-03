//
// Created by Aiden Williams on 29/04/2021.
//

#ifndef TEALANG_COMPILER_CPP20_TOKEN_H
#define TEALANG_COMPILER_CPP20_TOKEN_H

#include <string>
#include <regex>
#include <utility>

namespace lexer {
    // Defined according to pdf and state
    enum TOKEN_TYPE {
        // State 1
        // float
        TOK_FLOAT_TYPE          = 0,
        // int
        TOK_INT_TYPE            = 1,
        // bool
        TOK_BOOL_TYPE           = 2,
        //  string
        TOK_STRING_TYPE         = 3,
        // true
        TOK_TRUE                = 4,
        // false
        TOK_FALSE               = 5,
        // variable1 | _name
        TOK_IDENTIFIER          = 6,
        // and
        TOK_AND                 = 7,
        // or
        TOK_OR                  = 8,
        // not
        TOK_NOT                 = 9,
        // let
        TOK_LET                 = 10,
        // print
        TOK_PRINT               = 11,
        // return
        TOK_RETURN              = 12,
        // if
        TOK_IF                  = 13,
        // else
        TOK_ELSE                = 14,
        // for
        TOK_FOR                 = 15,
        // while
        TOK_WHILE               = 16,
        // State 3
        // "Printable"
        TOK_STRING              = 17,
        // State 6
        // 10, 12, 1
        TOK_INT                 = 18,
        // State 8
        // 10.12, 1.1
        TOK_FLOAT               = 19,
        // State 9
        // /
        TOK_DIVIDE              = 20,
        // State 11
        // //
        TOK_SINGLE_LINE_COMMENT = 21,
        // State 14
        // /* */
        TOK_MULTI_LINE_COMMENT  = 22,
        // State 15
        // EOF
        TOK_END                 = 23,
        // State 16
        // '{' | '}' | '(' | ')' | ',' | ':' | ';'
        // {
        TOK_OPENING_CURLY       = 24,
        // }
        TOK_CLOSING_CURLY       = 25,
        // (
        TOK_OPENING_CURVY       = 26,
        // )
        TOK_CLOSING_CURVY       = 27,
        // ,
        TOK_COMMA               = 28,
        // :
        TOK_COLON               = 29,
        // ;
        TOK_SEMICOLON           = 30,
        // State 17
        // -
        TOK_MINUS               = 31,
        // State 18
        // *
        TOK_ASTERISK            = 32,
        // State 19
        // +
        TOK_PLUS                = 33,
        // State 21
        // >
        TOK_MORE_THAN           = 34,
        // <
        TOK_LESS_THAN           = 35,
        // State 23
        // =
        TOK_EQUALS              = 36,
        // State 24
        // >=
        TOK_MORE_THAN_EQUAL_TO  = 37,
        // <=
        TOK_LESS_THAN_EQUAL_TO  = 38,
        // !=
        TOK_NOT_EQAUL_TO        = 39,
        // ==
        TOK_EQAUL_TO            = 40,
        // Invalid
        // Invalid
        TOK_INVALID             = 41,
    };

    /* regex statements that defines:
     * identifiers
     * strings
     * any literal
     * comments
     */
    extern std::regex identifier;
    extern std::regex string;
    extern std::regex intLiteral;
    extern std::regex floatLiteral;
    extern std::regex singleLineComment;
    extern std::regex multiLineComment;

    // Bool functions that confirm the passed string's proper state
    bool isFloatType(const std::string& s);
    bool isIntType(const std::string& s);
    bool isBoolType(const std::string& s);
    bool isStringType(const std::string& s);
    bool isTrue(const std::string& s);
    bool isFalse(const std::string& s);
    bool isIdentifier(const std::string& s);
    bool isAnd(const std::string& s);
    bool isOr(const std::string& s);
    bool isNot(const std::string& s);
    bool isLet(const std::string& s);
    bool isPrint(const std::string& s);
    bool isReturn(const std::string& s);
    bool isIf(const std::string& s);
    bool isElse(const std::string& s);
    bool isFor(const std::string& s);
    bool isWhile(const std::string& s);
    bool isString(const std::string& s);
    bool isInt(const std::string& s);
    bool isFloat(const std::string& s);
    bool isDivide(const std::string& s);
    bool isSingleLineComment(const std::string& s);
    bool isMultiLineComment(const std::string& s);
    bool isEnd(const std::string& s);
    bool isOpeningCurly(const std::string& s);
    bool isClosingCurly(const std::string& s);
    bool isOpeningCurvy(const std::string& s);
    bool isClosingCurvy(const std::string& s);
    bool isComma(const std::string& s);
    bool isColon(const std::string& s);
    bool isSemiColon(const std::string& s);
    bool isMinus(const std::string& s);
    bool isAsterisk(const std::string& s);
    bool isPlus(const std::string& s);
    bool isMoreThan(const std::string& s);
    bool isLessThan(const std::string& s);
    bool isEquals(const std::string& s);
    bool isMoreThanEqualTo(const std::string& s);
    bool isLessThanEqualTo(const std::string& s);
    bool isNotEqualTo(const std::string& s);
    bool isEqualTo(const std::string& s);

    // determines the token for an operator
    TOKEN_TYPE determineOperatorType(const std::string& op);

    // TOKEN_TYPE functions that provide possible token type building avenues from a particular state
    // some states do not have a dedicated Function as they are completely final states
    TOKEN_TYPE fromState1(const std::string& s);
    TOKEN_TYPE fromState3(const std::string& s);
    TOKEN_TYPE fromState6(const std::string& s);
    TOKEN_TYPE fromState8(const std::string& s);
    TOKEN_TYPE fromState9(const std::string& s);
    TOKEN_TYPE fromState11(const std::string& s);
    TOKEN_TYPE fromState14(const std::string& s);
    TOKEN_TYPE fromState15(const std::string& s);
    TOKEN_TYPE fromState16(const std::string& s);
    TOKEN_TYPE fromState17(const std::string& s);
    TOKEN_TYPE fromState18(const std::string& s);
    TOKEN_TYPE fromState19(const std::string& s);
    TOKEN_TYPE fromState21(const std::string& s);
    TOKEN_TYPE fromState22(const std::string& s);
    TOKEN_TYPE fromState23(const std::string& s);
    TOKEN_TYPE fromState25(const std::string& s);

    // Token class
    class Token {
    private:
        // TOKEN_TYPE Function that determines the TOKEN_TYPE for a string given a state
        static TOKEN_TYPE determineTokenType(std::string& s, unsigned int state);
    public:
        // The token constructor requires a string for its value and type as well as a line number
        // for future use by the parser and visitor classes
        Token(std::string s, unsigned int state, unsigned int lineNumber) :
                type(determineTokenType(s, state)),
                value(s),
                lineNumber(lineNumber)
        {};
        // default copy constructor
        Token(Token const &t) = default;

        Token() = default;
        ~Token() = default;
        TOKEN_TYPE type;
        std::string value;
        unsigned int lineNumber{};

    };
}


#endif //TEALANG_COMPILER_CPP20_TOKEN_H
