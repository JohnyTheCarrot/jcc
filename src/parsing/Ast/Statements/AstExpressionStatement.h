//
// Created by johny on 11/12/23.
//

#ifndef JCC_ASTEXPRESSIONSTATEMENT_H
#define JCC_ASTEXPRESSIONSTATEMENT_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstExpressionStatement final : public AstNode {
        AstExpressionStatement(const Span &span, std::unique_ptr<AstNode> &&expression)
                : AstNode(AstNodeType::ExpressionStatement)
                , _expression{ std::move(expression) } {
            this->_span = span;
        }

        [[nodiscard]]
        static std::optional<AstExpressionStatement> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _expression;
    };

} // parsing

#endif //JCC_ASTEXPRESSIONSTATEMENT_H
