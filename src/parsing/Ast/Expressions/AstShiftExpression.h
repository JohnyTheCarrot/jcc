//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTSHIFTEXPRESSION_H
#define JCC_ASTSHIFTEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

	enum class ShiftOperator { ShiftLeft, ShiftRight };

	struct AstShiftExpression final : public AstNode {
		AstShiftExpression(AstNode::Ptr &&left, ShiftOperator additiveOperator, AstNode::Ptr &&right)
			: AstNode(AstNodeType::ShiftExpression, Hierarchies::ShiftExpression)
			, _left{std::move(left)}
			, _shiftOperator{additiveOperator}
			, _right{std::move(right)} {}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _left;
		ShiftOperator _shiftOperator;
		AstNode::Ptr _right;
	};

}// namespace parsing

#endif//JCC_ASTSHIFTEXPRESSION_H
