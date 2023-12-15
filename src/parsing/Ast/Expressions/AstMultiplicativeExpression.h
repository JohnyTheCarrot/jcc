//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTMULTIPLICATIVEEXPRESSION_H
#define JCC_ASTMULTIPLICATIVEEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

	enum class MultiplicativeOperator { Multiply, Divide, Modulo };

	struct AstMultiplicativeExpression final : public AstNode {
		AstMultiplicativeExpression(
				AstNode::Ptr &&left, MultiplicativeOperator multiplicativeOperator, AstNode::Ptr &&right
		)
			: AstNode(AstNodeType::MultiplicativeExpression, Hierarchies::MultiplicativeExpression)
			, _left{std::move(left)}
			, _multiplicativeOperator{multiplicativeOperator}
			, _right{std::move(right)} {}

		[[nodiscard]]

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _left;
		MultiplicativeOperator _multiplicativeOperator;
		AstNode::Ptr _right;
	};

}// namespace parsing

#endif//JCC_ASTMULTIPLICATIVEEXPRESSION_H
