//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTRELATIONALEXPRESSION_H
#define JCC_ASTRELATIONALEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    enum class RelationalOperator {
        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual
    };

    struct AstRelationalExpression final : public AstNode {
        AstRelationalExpression(std::unique_ptr<AstNode> &&left, RelationalOperator relationalOperator, std::unique_ptr<AstNode> &&right)
            : AstNode(AstNodeType::RelationalExpression, Hierarchies::RelationalExpression)
            , _left{ std::move(left) }
            , _relationalOperator{ relationalOperator }
            , _right{ std::move(right) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        RelationalOperator _relationalOperator;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTRELATIONALEXPRESSION_H
