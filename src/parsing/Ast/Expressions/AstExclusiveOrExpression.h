//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTEXCLUSIVEOREXPRESSION_H
#define JCC_ASTEXCLUSIVEOREXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

    struct AstExclusiveOrExpression final : public AstNode {
        AstExclusiveOrExpression(AstNode::Ptr &&left, AstNode::Ptr &&right)
            : AstNode(AstNodeType::ExclusiveOr, Hierarchies::ExclusiveOrExpression)
            , _left{ std::move(left) }
            , _right{ std::move(right) }
        {}

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstNode::Ptr _left;
        AstNode::Ptr _right;
    };

} // parsing

#endif //JCC_ASTEXCLUSIVEOREXPRESSION_H
