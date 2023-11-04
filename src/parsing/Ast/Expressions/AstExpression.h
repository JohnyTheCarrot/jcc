//
// Created by johny on 11/2/23.
//

#ifndef JCC_ASTEXPRESSION_H
#define JCC_ASTEXPRESSION_H

#include <memory>
#include <vector>
#include "../../AstNode.h"

namespace parsing {

    struct AstExpression final : public AstNode {
        AstExpression(std::vector<std::unique_ptr<AstNode>> &&expressions)
            : AstNode(AstNodeType::Expression, Hierarchies::AssignmentExpression)
            , _expressions{ std::move(expressions) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::vector<std::unique_ptr<AstNode>> _expressions;
    };

} // parsing

#endif //JCC_ASTEXPRESSION_H
