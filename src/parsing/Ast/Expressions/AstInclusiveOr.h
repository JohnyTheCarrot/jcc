//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTINCLUSIVEOR_H
#define JCC_ASTINCLUSIVEOR_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstInclusiveOr final : public AstNode {
        AstInclusiveOr(std::unique_ptr<AstNode> left, std::unique_ptr<AstNode> right)
            : AstNode(AstNodeType::InclusiveOr)
            , _left{ std::move(left) }
            , _right{ std::move(right) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTINCLUSIVEOR_H
