//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTEQUALITYEXPRESSION_H
#define JCC_ASTEQUALITYEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    enum class EqualityOperator {
        Equal,
        NotEqual
    };

    struct AstEqualityExpression final : public AstNode {
        AstEqualityExpression(std::unique_ptr<AstNode> &&left, EqualityOperator equalityOperator, std::unique_ptr<AstNode> &&right)
            : AstNode(AstNodeType::EqualityExpression, Hierarchies::EqualityExpression)
            , _left{ std::move(left) }
            , _equalityOperator{ equalityOperator }
            , _right{ std::move(right) }
        {};

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser& parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        EqualityOperator _equalityOperator;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTEQUALITYEXPRESSION_H
