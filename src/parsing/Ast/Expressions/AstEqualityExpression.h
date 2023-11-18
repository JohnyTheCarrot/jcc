//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTEQUALITYEXPRESSION_H
#define JCC_ASTEQUALITYEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

    enum class EqualityOperator {
        Equal,
        NotEqual
    };

    struct AstEqualityExpression final : public AstNode {
        AstEqualityExpression(AstNode::Ptr &&left, EqualityOperator equalityOperator, AstNode::Ptr &&right)
            : AstNode(AstNodeType::EqualityExpression, Hierarchies::EqualityExpression)
            , _left{ std::move(left) }
            , _equalityOperator{ equalityOperator }
            , _right{ std::move(right) }
        {};

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser& parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstNode::Ptr _left;
        EqualityOperator _equalityOperator;
        AstNode::Ptr _right;
    };

} // parsing

#endif //JCC_ASTEQUALITYEXPRESSION_H
