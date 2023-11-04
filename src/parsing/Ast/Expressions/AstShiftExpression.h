//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTSHIFTEXPRESSION_H
#define JCC_ASTSHIFTEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    enum class ShiftOperator {
        ShiftLeft,
        ShiftRight
    };

    struct AstShiftExpression final : public AstNode {
        AstShiftExpression(std::unique_ptr<AstNode> &&left, ShiftOperator additiveOperator, std::unique_ptr<AstNode> &&right)
            : AstNode(AstNodeType::ShiftExpression, Hierarchies::ShiftExpression)
            , _left{ std::move(left) }
            , _shiftOperator{ additiveOperator }
            , _right{ std::move(right) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        ShiftOperator _shiftOperator;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTSHIFTEXPRESSION_H
