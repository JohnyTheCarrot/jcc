//
// Created by johny on 11/4/23.
//

#include "AstLogicalAndExpression.h"
#include "../../Parser.h"
#include "AstInclusiveOrExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstLogicalAndExpression::Parse(Parser &parser) {
		AstNode::Ptr left{AstInclusiveOrExpression::Parse(parser)};

		if (left == nullptr)
			return nullptr;

		while (true) {
			if (!parser)
				return left;

			const Token &token{parser.PeekNextToken()};

			if (token._type != TokenType::LogicalAnd)
				return left;

			parser.AdvanceCursor();

			AstNode::Ptr right{AstInclusiveOrExpression::Parse(parser)};

			if (right == nullptr)
				parser.Error(token._span, "Expected rhs expression");

			left = std::make_unique<AstLogicalAndExpression>(std::move(left), std::move(right));
		}
	}

	std::string AstLogicalAndExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstLogicalAndExpression, depth,
				{TOSTRING_FIELD_NODE("left", *_left) TOSTRING_FIELD_NODE("right", *_right)}
		)
	}
}// namespace parsing
