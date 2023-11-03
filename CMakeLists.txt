cmake_minimum_required(VERSION 3.22)
project(jcc)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/frozen)

add_subdirectory(libs/fmt)

add_executable(jcc src/main.cpp
        src/tokenizer.cpp
        src/tokenizer.h
        libs/magic_enum/magic_enum.hpp
        src/reporting.h
        src/Span.h
        src/Span.cpp
        src/reporting.cpp
        src/parsing/Parser.cpp
        src/parsing/Parser.h
        src/parsing/AstNode.h
        src/parsing/Ast/AstIdentifier.cpp
        src/parsing/Ast/AstIdentifier.h
        src/parsing/Ast/AstConstant.cpp
        src/parsing/Ast/AstConstant.h
        src/parsing/Ast/AstPrimaryExpression.cpp
        src/parsing/Ast/AstPrimaryExpression.h
        src/parsing/Ast/AstExpression.cpp
        src/parsing/Ast/AstExpression.h
        src/parsing/Ast/AstStringLiteral.cpp
        src/parsing/Ast/AstStringLiteral.h
        src/parsing/Ast/AstPostfixExpression.cpp
        src/parsing/Ast/AstPostfixExpression.h)

target_link_libraries(jcc PUBLIC frozen fmt)
