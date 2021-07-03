#pragma clang diagnostic push
// ignores warning for declaring regex variables below
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by Aiden Williams on 25/04/2021.
//

#include "Lexer.h"

namespace lexer {
    std::regex letter("[A-Za-z]");
    std::regex digit("[0-9]");
    std::regex printable("[\\x20-\\x7E]");

    bool isLetter(char c) {
        std::string _c;
        _c.push_back(c);
        return (std::regex_match(_c, letter));
    }

    bool isDigit(char c) {
        std::string _c;
        _c.push_back(c);
        return (std::regex_match(_c, digit));
    }

    bool isPrintable(char c) {
        std::string _c;
        _c.push_back(c);
        return (std::regex_match(_c, printable));
    }

    bool isFullstop(char c) {
        return c == '.';
    }

    bool isUnderscore(char c) {
        return c == '_';
    }

    bool isAsterisk(char c) {
        return c == '*';
    }

    bool isPlus(char c) {
        return c == '+';
    }

    bool isRelational(char c) {
        return c == '<' || c == '>';
    }

    bool isMinus(char c) {
        return c == '-';
    }

    bool isForwardSlash(char c) {
        return c == '/';
    }

    bool isBackSlash(char c) {
        return c == '\\';
    }

    bool isPunctuation(char c) {
        return c == '{' || c == '}' || c == '(' || c == ')' || c == ',' || c == ':' || c == ';';
    }

    bool isQuatiationMark(char c) {
        return c == '"';
    }

    bool isNewline(char c) {
        return c == '\n';
    }

    bool isEquals(char c) {
        return c == '=';
    }

    bool isExclamation(char c) {
        return c == '!';
    }

    bool isSpace(char c) {
        return c == ' ' || c == '\t';
    }

    bool isClosingCurly(char c) {
        return c == '}';
    }

    TRANSITION_TYPE determineTransitionType(char c) {
        if (isLetter(c)) return LETTER;
        if (isDigit(c)) return DIGIT;
        if (isFullstop(c)) return FULLSTOP;
        if (isUnderscore(c)) return UNDERSCORE;
        if (isAsterisk(c)) return ASTERISK;
        if (isPlus(c)) return PLUS;
        if (isRelational(c)) return RELATIONAL;
        if (isMinus(c)) return MINUS;
        if (isForwardSlash(c)) return FORWARD_SLASH;
        if (isBackSlash(c)) return BACK_SLASH;
        if (isClosingCurly(c)) return CLOSING_CURLY;
        if (isPunctuation(c)) return PUNCTUATION;
        if (isQuatiationMark(c)) return QUOTATION_MARK;
        if (isNewline(c)) return NEWLINE;
        if (isEquals(c)) return EQUALS;
        if (isExclamation(c)) return EXCLAMATION;
        if (isSpace(c)) return SPACE;
        // lastly
        if (isPrintable(c)) return PRINTABLE;
        return INVALID;
    }

    unsigned int delta(unsigned int fromState, char c) {
        unsigned int current_state;
        switch (determineTransitionType(c)) {
            case LETTER:
                (fromState == 0 || fromState == 1)
                ? current_state = 1 : current_state = 24;
                break;
            case DIGIT:
                if (fromState == 1) {
                    current_state = 1;
                } else if (fromState == 0 || fromState == 6) {
                    current_state = 6;
                } else if (fromState == 7 || fromState == 8) {
                    current_state = 8;
                } else {
                    current_state = 24;
                }
                break;
            case PRINTABLE:
                if (fromState == 2 || fromState == 4 || fromState == 5) {
                    if (!isBackSlash(c) and !isQuatiationMark(c)) {
                        current_state = 2;
                    } else {
                        current_state = 24;
                    }
                } else if (fromState == 10) {
                    current_state = 10;
                } else if (fromState == 12) {
                    if (!isAsterisk(c)) {
                        current_state = 12;
                    } else {
                        current_state = 24;
                    }
                } else if (fromState == 13) {
                    if (!isAsterisk(c) and !isForwardSlash(c)) {
                        current_state = 12;
                    } else {
                        current_state = 24;
                    }
                } else {
                    current_state = 24;
                }
                break;
            case FULLSTOP:
                (fromState == 6)
                ? current_state = 7 : current_state = 24;
                break;
            case UNDERSCORE:
                (fromState == 0 || fromState == 1)
                ? current_state = 1 : current_state = 24;
                break;
            case ASTERISK:
                if (fromState == 0) {
                    current_state = 18;
                } else if (fromState == 9) {
                    current_state = 12;
                } else if (fromState == 12) {
                    current_state = 13;
                } else {
                    current_state = 24;
                }
                break;
            case PLUS:
                (fromState == 0)
                ? current_state = 19 : current_state = 24;
                break;
            case RELATIONAL:
                (fromState == 0)
                ? current_state = 21 : current_state = 24;
                break;
            case MINUS:
                (fromState == 0)
                ? current_state = 17 : current_state = 24;
                break;
            case FORWARD_SLASH:
                if (fromState == 0) {
                    current_state = 9;
                } else if (fromState == 9) {
                    current_state = 10;
                } else if (fromState == 13) {
                    current_state = 14;
                } else {
                    current_state = 24;
                }
                break;
            case BACK_SLASH:
                (fromState == 2 || fromState == 4 || fromState == 5)
                ? current_state = 4 : current_state = 24;
                break;
            case CLOSING_CURLY:
                (fromState == 0)
                ? current_state = 25 : current_state = 24;
                break;
            case PUNCTUATION:
                (fromState == 0)
                ? current_state = 16 : current_state = 24;
                break;
            case QUOTATION_MARK:
                if (fromState == 0) {
                    current_state = 2;
                } else if (fromState == 2 || fromState == 5) {
                    current_state = 3;
                } else if (fromState == 4) {
                    current_state = 5;
                } else {
                    current_state = 24;
                }
                break;
            case NEWLINE:
                if (fromState == 10) {
                    current_state = 11;
                } else if (fromState == 12) {
                    current_state = 12;
                } else if (fromState == 13) {
                    current_state = 13;
                } else if (fromState == 0) {
                    current_state = 0;
                    break;
                } else {
                    current_state = 24;
                }
                break;
            case EQUALS:
                if (fromState == 0) {
                    current_state = 22;
                } else if (fromState == 22 || fromState == 21 || fromState == 20) {
                    current_state = 23;
                } else {
                    current_state = 24;
                }
                break;
            case EXCLAMATION:
                (fromState == 0)
                ? current_state = 20 : current_state = 24;
                break;
            case END:
                current_state = 15;
                break;
            case SPACE:
                if (fromState == 0) {
                    current_state = 0;
                    break;
                }
            case INVALID:
                current_state = 24;
        }
        // Check for printable again
        if (current_state == 24) {
            if (fromState == 2 || fromState == 4 || fromState == 5) {
                if (!isBackSlash(c) and !isQuatiationMark(c)) {
                    current_state = 2;
                } else {
                    current_state = 24;
                }
            } else if (fromState == 10) {
                current_state = 10;
            } else if (fromState == 12) {
                if (!isAsterisk(c)) {
                    current_state = 12;
                } else {
                    current_state = 24;
                }
            } else if (fromState == 13) {
                if (!isAsterisk(c) and !isForwardSlash(c)) {
                    current_state = 12;
                } else {
                    current_state = 24;
                }
            } else {
                current_state = 24;
            }
        }
        return current_state;
    }

    std::vector <Token> Lexer::extractLexemes(const std::string &text) {
        std::vector <Token> ret;
        std::string value;
        unsigned int previous_state, current_state = 0;
        unsigned int lineNumber = 1;
        // Go over every character in text
        for (auto c : text) {
            // Do DFSA transition
            previous_state = current_state;
            current_state = delta(previous_state, c);
            // Is the current state
            if (current_state == 24) {
                // Confirm previous state was a final state
                if (!finalStates[previous_state])
                    throw std::runtime_error("Lexical error on line " + std::to_string(lineNumber) + ".");
                // Create a token for value with given its final state
                // Do not add an empty token
                if (!value.empty()){
                    Token t(value, previous_state, lineNumber);
                    // Store this token t
                    ret.emplace_back(t);
                }
                //reset
                current_state = delta(0, c);
                value = "";
                // if current state moves to state again 24, go back to state 0
                if (current_state == 24) current_state = 0;
                    // else if the current state is not 0 add c to value
                else if (current_state != 0) value += c;
            } else {
                // if the current state is not 0 add c to value
                if (current_state != 0) value += c;
            }
            // If c is a new line char '\n' increase the lineNumber
            if (isNewline(c)) lineNumber++;
        }
        // If any of the states aren't final there is a lexical error
        if (!finalStates[current_state] && !finalStates[previous_state])
            throw std::runtime_error("Lexical error on line " + std::to_string(lineNumber) + ".");
        // Helper case for when a single line comment doesnt end with a new line char
        if(current_state == 10){
            value += "\n";
            current_state = 11;
        }
        // emplace token so far with current state
        ret.emplace_back(Token(value, current_state, lineNumber));
        // emplace End token
        ret.emplace_back(Token("", 15, lineNumber));
        // set tokens = ret {vector of tokens}
        tokens = ret;
        // return ret {vector of tokens}
        return ret;
    }
}
#pragma clang diagnostic pop