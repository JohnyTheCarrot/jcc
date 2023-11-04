//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTASSIGNMENTEXPRESSION_H
#define JCC_ASTASSIGNMENTEXPRESSION_H

#include <memory>
#include "../../AstNode.h"

namespace parsing {

    enum class AstAssignmentOperator {
        Assign,
        MultiplyAssign,
        DivideAssign,
        ModuloAssign,
        PlusAssign,
        MinusAssign,
        ShiftLeftAssign,
        ShiftRightAssign,
        BitwiseAndAssign,
        BitwiseXorAssign,
        BitwiseOrAssign,
    };

    struct AstAssignmentExpression final : public AstNode {
        AstAssignmentExpression(std::unique_ptr<AstNode> &&left, AstAssignmentOperator assignmentOperator, std::unique_ptr<AstNode> &&right)
            : AstNode(AstNodeType::AssignmentExpression, Hierarchies::AssignmentExpression)
            , _left{ std::move(left) }
            , _assignmentOperator{ assignmentOperator }
            , _right{ std::move(right) }
        {}

        [[nodiscard]]
        static std::unique_ptr<AstNode> Parse(Parser &parser);

        [[nodiscard]]
        std::string ToString(size_t depth) const override;

        std::unique_ptr<AstNode> _left;
        AstAssignmentOperator _assignmentOperator;
        std::unique_ptr<AstNode> _right;
    };

} // parsing

#endif //JCC_ASTASSIGNMENTEXPRESSION_H
