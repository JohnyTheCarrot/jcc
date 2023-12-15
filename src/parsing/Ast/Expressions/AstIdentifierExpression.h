//
// Created by johny on 10/18/23.
//

#ifndef JCC_ASTIDENTIFIEREXPRESSION_H
#define JCC_ASTIDENTIFIEREXPRESSION_H

#include "../../AstNode.h"
#include <string>

class Parser;

namespace parsing {

	struct AstIdentifierExpression final : public parsing::AstNode {
		explicit AstIdentifierExpression(std::string &&ident)
			: AstNode(AstNodeType::Identifier, Hierarchies::PrimaryExpression)
			, _ident{std::move(ident)} {};

		static std::optional<AstIdentifierExpression> Parse(Parser &);

		[[nodiscard]]
		std::string ToString(size_t depth) const override;

		std::string _ident;
	};

}// namespace parsing


#endif//JCC_ASTIDENTIFIEREXPRESSION_H
