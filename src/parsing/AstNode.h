//
// Created by johny on 10/18/23.
//

#ifndef JCC_ASTNODE_H
#define JCC_ASTNODE_H

#include <optional>
#include <iostream>
#include "../Span.h"

#define TODO() { \
    std::cerr << "TODO: " << __FILE__ << ":" << __LINE__ << std::endl; \
    std::exit(1);           \
    __builtin_unreachable(); \
}

class Parser;

namespace parsing {
    enum class AstNodeType {
        Identifier,
        Constant,
        StringLiteral,
        PostfixExpression,
        UnaryExpression,
        CastExpression,
        TypeSpecifier,
        TypeQualifier,
        SpecifierQualifierList,
        TypeName,
        MultiplicativeExpression,
        AdditiveExpression,
        ShiftExpression,
        RelationalExpression,
        EqualityExpression,
        AndExpression,
        ExclusiveOr,
        InclusiveOr,
        LogicalAnd,
        LogicalOr,
        ConditionalExpression,
        AssignmentExpression,
        Expression,
        StaticAssertDeclaration,
        StorageClassSpecifier,
        FunctionSpecifier,
        AlignmentSpecifier,
        DeclarationSpecifiers,
        TypeQualifierList,
        Pointer,
        Declarator,
        InitDeclarator,
        InitDeclaratorList,
        Declaration,
        DirectDeclarator,
        ParameterDeclaration,
        ParameterList,
        ParameterTypeList,
        Designator,
        DesignatorList,
        Designation,
        InitializerList,
        Initializer,
    };

    struct AstNode {
        // todo: add span to constructor
        AstNode(AstNodeType type, int hierarchyLevel)
            : _type{ type }
            , _span{}
            , _hierarchyLevel{ hierarchyLevel }
        {};

        explicit AstNode(AstNodeType type)
            : AstNode(type, Hierarchies::NotApplicable)
        {};

        AstNodeType _type;
        Span _span;
        int _hierarchyLevel;

        // Hierarchy levels
        struct Hierarchies {
            // Expression hierarchy
            static constexpr int PrimaryExpression          {  0 };
            static constexpr int PostfixExpression          {  1 };
            static constexpr int UnaryExpression            {  2 };
            static constexpr int CastExpression             {  3 };
            static constexpr int MultiplicativeExpression   {  4 };
            static constexpr int AdditiveExpression         {  5 };
            static constexpr int ShiftExpression            {  6 };
            static constexpr int RelationalExpression       {  7 };
            static constexpr int EqualityExpression         {  8 };
            static constexpr int AndExpression              {  9 };
            static constexpr int ExclusiveOrExpression      { 10 };
            static constexpr int InclusiveOrExpression      { 11 };
            static constexpr int LogicalAndExpression       { 12 };
            static constexpr int LogicalOrExpression        { 13 };
            static constexpr int ConditionalExpression      { 14 };
            static constexpr int AssignmentExpression       { 15 };

            // TODO: Declaration hierarchy
            static constexpr int NotApplicable              { -1 };
            static constexpr int Declaration                {  0 };
            static constexpr int TypeSpecifier              {  0 };
            static constexpr int TypeQualifier              {  0 };
            static constexpr int SpecifierQualifierList     {  1 };
            static constexpr int TypeName                   {  2 };
        };

        virtual ~AstNode() = default;

        [[nodiscard]]
        virtual std::string ToString(size_t depth) const = 0;

        static std::string Indent(size_t depth) {
            std::string indent{};
            for (size_t i = 0; i < depth; ++i) {
                indent += "  ";
            }
            return indent;
        }
    };
} // parsing

#endif //JCC_ASTNODE_H
