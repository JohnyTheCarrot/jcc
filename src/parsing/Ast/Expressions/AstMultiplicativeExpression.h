//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTMULTIPLICATIVEEXPRESSION_H
#define JCC_ASTMULTIPLICATIVEEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    enum class MultiplicativeOperator {
        Multiply,
        Divide,
        Modulo
    };

    struct AstMultiplicativeExpression final : public AstNode {
        AstMultiplicativeExpression(std::unique_ptr<AstNode> &&left, MultiplicativeOperator multiplicativeOperator, std::unique_ptr<AstNode> &&right)
            : AstNode(AstNodeType::MultiplicativeExpression, Hierarchies::MultiplicativeExpression)
            , _left{ std::move(left) }
            , _multiplicativeOperator{ multiplicativeOperator }
            , _right{ std::move(right) }
        {}

        [[nodiscard]]

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        MultiplicativeOperator _multiplicativeOperator;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTMULTIPLICATIVEEXPRESSION_H
