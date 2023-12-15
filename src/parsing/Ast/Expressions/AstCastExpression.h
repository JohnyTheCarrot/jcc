//
// Created by johny on 11/3/23.
//

#ifndef JCC_ASTCASTEXPRESSION_H
#define JCC_ASTCASTEXPRESSION_H

#include "../../AstNode.h"
#include "../Declarations/AstTypeName.h"
#include "AstIdentifierExpression.h"

namespace parsing {

	struct AstCastExpression final : public AstNode {
		AstCastExpression(std::optional<AstTypeName> typeName, AstNode::Ptr &&expression)
			: AstNode(AstNodeType::CastExpression, Hierarchies::CastExpression)
			, _typeName{std::move(typeName)}
			, _expression{std::move(expression)} {};

		std::optional<AstTypeName> _typeName;
		AstNode::Ptr _expression;

		[[nodiscard]]
		static AstNode::Ptr Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;
	};

}// namespace parsing

#endif//JCC_ASTCASTEXPRESSION_H
