//
// Created by johny on 11/4/23.
//

#ifndef JCC_ASTSTATICASSERTDECLARATION_H
#define JCC_ASTSTATICASSERTDECLARATION_H

#include "../../AstNode.h"
#include "../Expressions/AstStringLiteralExpression.h"

namespace parsing {

	struct AstStaticAssertDeclaration final : public AstNode {
		AstStaticAssertDeclaration(AstNode::Ptr &&constantExpression, AstStringLiteralExpression &&message)
			: AstNode(AstNodeType::StaticAssertDeclaration, Hierarchies::Declaration)
			, _constantExpression{std::move(constantExpression)}
			, _message{std::move(message)} {}

		[[nodiscard]]
		static std::optional<AstStaticAssertDeclaration> Parse(Parser &parser);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		AstNode::Ptr _constantExpression;
		AstStringLiteralExpression _message;
	};

}// namespace parsing

#endif//JCC_ASTSTATICASSERTDECLARATION_H
