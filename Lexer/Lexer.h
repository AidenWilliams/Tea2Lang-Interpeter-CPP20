//
// Created by Aiden Williams on 25/04/2021.
//

#ifndef TEALANG_COMPILER_CPP20_LEXER_H
#define TEALANG_COMPILER_CPP20_LEXER_H

#include <string>
#include <vector>
#include <regex>
#include <stdexcept>
#include "Token.h"


namespace lexer {

    enum TRANSITION_TYPE {
        // [A-Za-z]
        LETTER          =  0,
        // [0-9]
        DIGIT           =  1,
        // [\x20-\x7E]
        PRINTABLE       =  2,
        // '.'
        FULLSTOP        =  3,
        // '_'
        UNDERSCORE      =  4,
        // ‘*’
        ASTERISK        =  5,
        // ‘+’
        PLUS            =  6,
        // ‘<’ | ‘>’
        RELATIONAL      =  7,
        // ‘-’
        MINUS           =  8,
        // ‘/’
        FORWARD_SLASH   =  9,
        // ‘\’
        BACK_SLASH      =  10,
        // '{' | '}' | '(' | ')' | ',' | ':' | ';'
        PUNCTUATION     = 11,
        // '"'
        QUOTATION_MARK  = 12,
        // '\n'
        NEWLINE         = 13,
        // '='
        EQUALS          = 14,
        // '!'
        EXCLAMATION     = 15,
        END             = 16,
        INVALID         = 17,
        SPACE         = 18,
        CLOSING_CURLY         = 19
    };
    // regex statements that define a letter, digit, or printable character
    extern std::regex letter;
    extern std::regex digit;
    extern std::regex printable;

    // bool functions that determine the char type
    bool isLetter(char c);
    bool isDigit(char c);
    bool isPrintable(char c);
    bool isFullstop(char c);
    bool isUnderscore(char c);
    bool isAsterisk(char c);
    bool isPlus(char c);
    bool isRelational(char c);
    bool isMinus(char c);
    bool isForwardSlash(char c);
    bool isBackSlash(char c);
    bool isPunctuation(char c);
    bool isQuatiationMark(char c);
    bool isNewline(char c);
    bool isEquals(char c);
    bool isExclamation(char c);
    bool isSpace(char c);
    bool isClosingCurly(char c);

    // TRANSITION_TYPE Function that determines the TRANSITION_TYPE when given a char c
    static TRANSITION_TYPE determineTransitionType(char c);
    // delta Function for the DFSA designed for this lexer
    unsigned int delta(unsigned int fromState, char c);

    // Lexer class
    class Lexer {
    public:
        Lexer() = default;
        ~Lexer() = default;
        // Vector of the program tokens
        std::vector<Token> tokens;
        // Vector of final states
        // When used, given a vector it acts as a Function to determine whether a state is final or not
        // e.g finalStates[0] == true
        std::vector<bool> finalStates = {
                                            true, true, false, true, false, false, true, false, true, true, true, true,
                                            false, false, true, true, true, true, true, true, false, true, true, true,
                                            true, true
                                        };
        // Function to extract program lexemes into tokens
        std::vector<Token> extractLexemes(const std::string& text);

    };
};




#endif //TEALANG_COMPILER_CPP20_LEXER_H
