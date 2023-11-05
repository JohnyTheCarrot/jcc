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
        src/parsing/Ast/Expressions/AstIdentifierExpression.cpp
        src/parsing/Ast/Expressions/AstIdentifierExpression.h
        src/parsing/Ast/Expressions/AstNumericalConstantExpression.cpp
        src/parsing/Ast/Expressions/AstNumericalConstantExpression.h
        src/parsing/Ast/Expressions/AstPrimaryExpression.cpp
        src/parsing/Ast/Expressions/AstPrimaryExpression.h
        src/parsing/Ast/Expressions/AstExpression.cpp
        src/parsing/Ast/Expressions/AstExpression.h
        src/parsing/Ast/Expressions/AstStringLiteralExpression.cpp
        src/parsing/Ast/Expressions/AstStringLiteralExpression.h
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
        src/parsing/Ast/Declarations/AstTypeQualifier.h
        src/parsing/Ast/Expressions/AstMultiplicativeExpression.cpp
        src/parsing/Ast/Expressions/AstMultiplicativeExpression.h
        src/parsing/Ast/Expressions/AstAdditiveExpression.cpp
        src/parsing/Ast/Expressions/AstAdditiveExpression.h
        src/parsing/Ast/Expressions/AstShiftExpression.cpp
        src/parsing/Ast/Expressions/AstShiftExpression.h
        src/parsing/Ast/Expressions/AstRelationalExpression.cpp
        src/parsing/Ast/Expressions/AstRelationalExpression.h
        src/parsing/Ast/Expressions/AstEqualityExpression.cpp
        src/parsing/Ast/Expressions/AstEqualityExpression.h
        src/parsing/Ast/Expressions/AstAndExpression.cpp
        src/parsing/Ast/Expressions/AstAndExpression.h
        src/parsing/Ast/Expressions/AstExclusiveOrExpression.cpp
        src/parsing/Ast/Expressions/AstExclusiveOrExpression.h
        src/parsing/Ast/Expressions/AstInclusiveOrExpression.cpp
        src/parsing/Ast/Expressions/AstInclusiveOrExpression.h
        src/parsing/Ast/Expressions/AstLogicalAndExpression.cpp
        src/parsing/Ast/Expressions/AstLogicalAndExpression.h
        src/parsing/Ast/Expressions/AstLogicalOrExpression.cpp
        src/parsing/Ast/Expressions/AstLogicalOrExpression.h
        src/parsing/Ast/Expressions/AstConditionalExpression.cpp
        src/parsing/Ast/Expressions/AstConditionalExpression.h
        src/parsing/Ast/Expressions/AstAssignmentExpression.cpp
        src/parsing/Ast/Expressions/AstAssignmentExpression.h
        src/parsing/Ast/Declarations/AstDeclaration.cpp
        src/parsing/Ast/Declarations/AstDeclaration.h
        src/parsing/Ast/Declarations/AstStaticAssertDeclaration.cpp
        src/parsing/Ast/Declarations/AstStaticAssertDeclaration.h
        src/parsing/Ast/Expressions/AstConstantExpression.cpp
        src/parsing/Ast/Expressions/AstConstantExpression.h
        src/parsing/Ast/Declarations/AstDeclarationSpecifiers.cpp
        src/parsing/Ast/Declarations/AstDeclarationSpecifiers.h
        src/parsing/Ast/Declarations/AstStorageClassSpecifier.cpp
        src/parsing/Ast/Declarations/AstStorageClassSpecifier.h
        src/parsing/Ast/Declarations/AstFunctionSpecifier.cpp
        src/parsing/Ast/Declarations/AstFunctionSpecifier.h
        src/parsing/Ast/Declarations/AstAlignmentSpecifier.cpp
        src/parsing/Ast/Declarations/AstAlignmentSpecifier.h
        src/parsing/Ast/Declarations/AstDirectDeclarator.cpp
        src/parsing/Ast/Declarations/AstDirectDeclarator.h
        src/parsing/Ast/Declarations/AstDeclarator.cpp
        src/parsing/Ast/Declarations/AstDeclarator.h
        src/parsing/Ast/Declarations/AstTypeQualifierList.cpp
        src/parsing/Ast/Declarations/AstTypeQualifierList.h
        src/parsing/Ast/Declarations/AstPointer.cpp
        src/parsing/Ast/Declarations/AstPointer.h
        src/parsing/Ast/Declarations/AstInitDeclarator.cpp
        src/parsing/Ast/Declarations/AstInitDeclarator.h
        src/parsing/Ast/Declarations/AstInitDeclaratorList.cpp
        src/parsing/Ast/Declarations/AstInitDeclaratorList.h)

target_link_libraries(jcc PUBLIC frozen fmt)
