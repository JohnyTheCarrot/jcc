//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTPOSTFIXEXPRESSION_H
#define JCC_ASTPOSTFIXEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    enum class PostfixExpressionType {
        PrimaryExpression,
        FunctionCall,
        ArraySubscript,
        MemberAccess,
        PointerMemberAccess,
        PostIncrement,
        PostDecrement
    };

    struct AstPostfixExpression final : public AstNode {
        AstPostfixExpression(std::unique_ptr<AstNode> &&left, PostfixExpressionType type, std::unique_ptr<AstNode> &&right)
            : AstNode(AstNodeType::PostfixExpression, Hierarchies::PostfixExpression)
            , _left{ std::move(left) }
            , _postfixExpressionType{ type }
            , _right{ std::move(right) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        PostfixExpressionType _postfixExpressionType;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTPOSTFIXEXPRESSION_H
