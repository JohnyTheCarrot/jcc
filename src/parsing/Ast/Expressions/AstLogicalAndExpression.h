//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTLOGICALANDEXPRESSION_H
#define JCC_ASTLOGICALANDEXPRESSION_H

#include "../../AstNode.h"

namespace parsing {

	struct AstLogicalAndExpression final : public AstNode {
		AstLogicalAndExpression(AstNode::Ptr &&left, AstNode::Ptr &&right)
			: AstNode(AstNodeType::LogicalAnd, Hierarchies::LogicalAndExpression)
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

#endif//JCC_ASTLOGICALANDEXPRESSION_H
