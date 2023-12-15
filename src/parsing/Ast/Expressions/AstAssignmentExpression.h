//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTASSIGNMENTEXPRESSION_H
#define JCC_ASTASSIGNMENTEXPRESSION_H

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
		AstAssignmentExpression(AstNode::Ptr &&left, AstAssignmentOperator assignmentOperator, AstNode::Ptr &&right)
			: AstNode(AstNodeType::AssignmentExpression, Hierarchies::AssignmentExpression)
			, _left{std::move(left)}
			, _assignmentOperator{assignmentOperator}
			, _right{std::move(right)} {}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _left;
		AstAssignmentOperator _assignmentOperator;
		AstNode::Ptr _right;
	};

}// namespace parsing

#endif//JCC_ASTASSIGNMENTEXPRESSION_H
