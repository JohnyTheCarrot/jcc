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
        src/parsing/Ast/Expressions/AstIdentifier.cpp
        src/parsing/Ast/Expressions/AstIdentifier.h
        src/parsing/Ast/Expressions/AstConstant.cpp
        src/parsing/Ast/Expressions/AstConstant.h
        src/parsing/Ast/Expressions/AstPrimaryExpression.cpp
        src/parsing/Ast/Expressions/AstPrimaryExpression.h
        src/parsing/Ast/Expressions/AstExpression.cpp
        src/parsing/Ast/Expressions/AstExpression.h
        src/parsing/Ast/Expressions/AstStringLiteral.cpp
        src/parsing/Ast/Expressions/AstStringLiteral.h
        src/parsing/Ast/Expressions/AstPostfixExpression.cpp
        src/parsing/Ast/Expressions/AstPostfixExpression.h
        src/parsing/Ast/Expressions/AstUnaryExpression.cpp
        src/parsing/Ast/Expressions/AstUnaryExpression.h
        src/parsing/Ast/Expressions/AstCastExpression.cpp
        src/parsing/Ast/Expressions/AstCastExpression.h
        src/parsing/Ast/Declarations/AstTypeName.cpp
        src/parsing/Ast/Declarations/AstTypeName.h
        src/parsing/Ast/Declarations/AstTypeSpecifier.cpp
        src/parsing/Ast/Declarations/AstTypeSpecifier.h
        src/parsing/Ast/Declarations/AstSpecifierQualifierList.cpp
        src/parsing/Ast/Declarations/AstSpecifierQualifierList.h
        src/parsing/Ast/Declarations/AstTypeQualifier.cpp
        src/parsing/Ast/Declarations/AstTypeQualifier.h)

target_link_libraries(jcc PUBLIC frozen fmt)
