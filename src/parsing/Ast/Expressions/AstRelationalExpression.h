//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTRELATIONALEXPRESSION_H
#define JCC_ASTRELATIONALEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

    enum class RelationalOperator {
        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual
    };

    struct AstRelationalExpression final : public AstNode {
        AstRelationalExpression(AstNode::Ptr &&left, RelationalOperator relationalOperator, AstNode::Ptr &&right)
            : AstNode(AstNodeType::RelationalExpression, Hierarchies::RelationalExpression)
            , _left{ std::move(left) }
            , _relationalOperator{ relationalOperator }
            , _right{ std::move(right) }
        {}

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstNode::Ptr _left;
        RelationalOperator _relationalOperator;
        AstNode::Ptr _right;
    };

} // parsing

#endif //JCC_ASTRELATIONALEXPRESSION_H
