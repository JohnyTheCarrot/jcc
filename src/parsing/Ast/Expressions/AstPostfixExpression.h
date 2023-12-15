//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTPOSTFIXEXPRESSION_H
#define JCC_ASTPOSTFIXEXPRESSION_H

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
		AstPostfixExpression(AstNode::Ptr &&left, PostfixExpressionType type, AstNode::Ptr &&right)
			: AstNode(AstNodeType::PostfixExpression, Hierarchies::PostfixExpression)
			, _left{std::move(left)}
			, _postfixExpressionType{type}
			, _right{std::move(right)} {}

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _left;
		PostfixExpressionType _postfixExpressionType;
		AstNode::Ptr _right;
	};

}// namespace parsing

#endif//JCC_ASTPOSTFIXEXPRESSION_H
