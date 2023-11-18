//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTCONDITIONALEXPRESSION_H
#define JCC_ASTCONDITIONALEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

    struct AstConditionalExpression final : public AstNode {
        AstConditionalExpression(AstNode::Ptr &&condition, AstNode::Ptr &&trueExpression, AstNode::Ptr &&falseExpression)
            : AstNode(AstNodeType::ConditionalExpression, Hierarchies::ConditionalExpression)
            , _condition { std::move(condition) }
            , _trueExpression { std::move(trueExpression) }
            , _falseExpression { std::move(falseExpression) }
        {};

        [[nodiscard]]
        static AstNode::Ptr Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        AstNode::Ptr _condition;
        AstNode::Ptr _trueExpression;
        AstNode::Ptr _falseExpression;
    };

} // parsing

#endif //JCC_ASTCONDITIONALEXPRESSION_H
