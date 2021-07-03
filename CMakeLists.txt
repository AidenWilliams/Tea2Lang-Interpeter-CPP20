cmake_minimum_required(VERSION 3.16)
project(TeaLang_Compiler_CPP20)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-march=native")

set(SOURCES main.cpp Lexer/Lexer.cpp Lexer/Token.cpp Parser/Parser.cpp Parser/AST.cpp Visitor/XML_Visitor.cpp Visitor/Semantic_Visitor.cpp Visitor/Interpreter_Visitor.cpp)
set(HEADERS Lexer/Lexer.h Lexer/Token.h Parser/Parser.h Parser/AST.h Visitor/Visitor.h Visitor/XML_Visitor.h Visitor/Semantic_Visitor.h Visitor/Interpreter_Visitor.h)

add_executable(TeaLang ${SOURCES} ${HEADERS})
