//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTADDITIVEEXPRESSION_H
#define JCC_ASTADDITIVEEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

    enum class AdditiveOperator {
        Plus,
        Minus
    };

    struct AstAdditiveExpression final : public AstNode {
        AstAdditiveExpression(AstNode::Ptr &&left, AdditiveOperator additiveOperator, AstNode::Ptr &&right)
                : AstNode(AstNodeType::AdditiveExpression, Hierarchies::AdditiveExpression)
                , _left{ std::move(left) }
                , _additiveOperator{ additiveOperator }
                , _right{ std::move(right) }
        {}

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstNode::Ptr _left;
        AdditiveOperator _additiveOperator;
        AstNode::Ptr _right;
    };

} // parsing

#endif //JCC_ASTADDITIVEEXPRESSION_H
