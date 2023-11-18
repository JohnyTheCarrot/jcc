//
// Created by johny on 11/2/23.
//

#ifndef JCC_ASTEXPRESSION_H
#define JCC_ASTEXPRESSION_H

#include <vector>
#include "../../AstNode.h"

namespace parsing {

    struct AstExpression final : public AstNode {
        AstExpression(std::vector<AstNode::Ptr> &&expressions)
            : AstNode(AstNodeType::Expression, Hierarchies::AssignmentExpression)
            , _expressions{ std::move(expressions) }
        {}

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::vector<AstNode::Ptr> _expressions;
    };

} // parsing

#endif //JCC_ASTEXPRESSION_H
