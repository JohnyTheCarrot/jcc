//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTINCLUSIVEOREXPRESSION_H
#define JCC_ASTINCLUSIVEOREXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

	struct AstInclusiveOrExpression final : public AstNode {
		AstInclusiveOrExpression(AstNode::Ptr &&left, AstNode::Ptr &&right)
			: AstNode(AstNodeType::InclusiveOr, Hierarchies::InclusiveOrExpression)
			, _left{std::move(left)}
			, _right{std::move(right)} {}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _left;
		AstNode::Ptr _right;
	};

}// namespace parsing

#endif//JCC_ASTINCLUSIVEOREXPRESSION_H
