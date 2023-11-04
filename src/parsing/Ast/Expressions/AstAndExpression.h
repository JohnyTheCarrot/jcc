//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTANDEXPRESSION_H
#define JCC_ASTANDEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstAndExpression final : public AstNode {
        AstAndExpression(std::unique_ptr<AstNode> &&left, std::unique_ptr<AstNode> &&right)
            : AstNode(AstNodeType::AndExpression, Hierarchies::AndExpression)
            , _left{ std::move(left) }
            , _right{ std::move(right) } {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTANDEXPRESSION_H
