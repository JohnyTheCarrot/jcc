//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTUNARYEXPRESSION_H
#define JCC_ASTUNARYEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

	enum class AstUnaryOperator {
		Plus,
		Minus,
		Not,
		BitwiseNot,
		Dereference,
		AddressOf,
		SizeOf,
		Cast,
		PreIncrement,
		PreDecrement,
		PostIncrement,
		PostDecrement
	};

	struct AstUnaryExpression final : public AstNode {
		AstUnaryExpression(AstUnaryOperator unaryOperator, AstNode::Ptr &&operand)
			: AstNode(AstNodeType::UnaryExpression, Hierarchies::UnaryExpression)
			, _unaryOperator{unaryOperator}
			, _operand{std::move(operand)} {}

		[[nodiscard]]

		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstUnaryOperator _unaryOperator;
		AstNode::Ptr _operand;
	};

}// namespace parsing

#endif//JCC_ASTUNARYEXPRESSION_H
