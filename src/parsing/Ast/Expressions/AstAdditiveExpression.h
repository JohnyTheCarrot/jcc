//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTADDITIVEEXPRESSION_H
#define JCC_ASTADDITIVEEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    enum class AdditiveOperator {
        Plus,
        Minus
    };

    struct AstAdditiveExpression final : public AstNode {
        AstAdditiveExpression(std::unique_ptr<AstNode> &&left, AdditiveOperator additiveOperator, std::unique_ptr<AstNode> &&right)
                : AstNode(AstNodeType::AdditiveExpression)
                , _left{std::move(left) }
                , _additiveOperator{additiveOperator }
                , _right{std::move(right) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        AdditiveOperator _additiveOperator;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTADDITIVEEXPRESSION_H
