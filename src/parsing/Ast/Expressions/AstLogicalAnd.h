//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTLOGICALAND_H
#define JCC_ASTLOGICALAND_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    struct AstLogicalAnd final : public AstNode {
        AstLogicalAnd(std::unique_ptr<AstNode> left, std::unique_ptr<AstNode> right)
            : AstNode(AstNodeType::LogicalAnd)
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

#endif //JCC_ASTLOGICALAND_H