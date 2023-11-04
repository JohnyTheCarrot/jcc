//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTCONDITIONALEXPRESSION_H
#define JCC_ASTCONDITIONALEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstConditionalExpression final : public AstNode {
        AstConditionalExpression(std::unique_ptr<AstNode> &&condition, std::unique_ptr<AstNode> &&trueExpression, std::unique_ptr<AstNode> &&falseExpression)
            : AstNode(AstNodeType::ConditionalExpression)
            , _condition {std::move(condition) }
            , _trueExpression {std::move(trueExpression) }
            , _falseExpression {std::move(falseExpression) }
        {};

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _condition;
        std::unique_ptr<AstNode> _trueExpression;
        std::unique_ptr<AstNode> _falseExpression;
    };

} // parsing

#endif //JCC_ASTCONDITIONALEXPRESSION_H
