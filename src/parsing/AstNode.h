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
    };

    struct AstNode {
        explicit AstNode(AstNodeType type) : _type{ type } {};

        AstNodeType _type;
        Span _span;

        virtual ~AstNode() = default;

        [[nodiscard]]
        virtual std::string ToString(size_t depth) const = 0;

        static std::string Indent(size_t depth) {
            std::string indent{};
            for (size_t i = 0; i < depth; ++i) {
                indent += '\t';
            }
            return indent;
        }
    };
} // parsing

#endif //JCC_ASTNODE_H
