//
// Created by johny on 11/12/23.
//

#ifndef JCC_ASTEXPRESSIONSTATEMENT_H
#define JCC_ASTEXPRESSIONSTATEMENT_H

#include "../../AstNode.h"

namespace parsing {

    struct AstExpressionStatement final : public AstNode {
        AstExpressionStatement(const Span &span, AstNode::Ptr &&expression)
                : AstNode(AstNodeType::ExpressionStatement)
                , _expression{ std::move(expression) } {
            this->_span = span;
        }

        [[nodiscard]]
        static std::optional<AstExpressionStatement> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstNode::Ptr _expression;
    };

} // parsing

#endif //JCC_ASTEXPRESSIONSTATEMENT_H
