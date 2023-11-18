//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTLOGICALOREXPRESSION_H
#define JCC_ASTLOGICALOREXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

    struct AstLogicalOrExpression final : public AstNode {
        AstLogicalOrExpression(AstNode::Ptr &&left, AstNode::Ptr &&right)
            : AstNode(AstNodeType::LogicalOr, Hierarchies::LogicalOrExpression)
            , _left{ std::move(left) }
            , _right{ std::move(right) }
        { }

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstNode::Ptr _left;
        AstNode::Ptr _right;
    };

} // parsing

#endif //JCC_ASTLOGICALOREXPRESSION_H
