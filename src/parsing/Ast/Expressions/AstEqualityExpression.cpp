//
// Created by johny on 11/4/23.
//

#include "AstEqualityExpression.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../Parser.h"
#include "AstRelationalExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstEqualityExpression::Parse(Parser &parser) {
		AstNode::Ptr left{AstRelationalExpression::Parse(parser)};

		if (left == nullptr)
			return nullptr;

		while (true) {
			if (!parser)
				return left;

			const Token &token{parser.PeekNextToken()};

			EqualityOperator equalityOperator;

			switch (token._type) {
				case TokenType::Equals:
					equalityOperator = EqualityOperator::Equal;
					break;
				case TokenType::NotEquals:
					equalityOperator = EqualityOperator::NotEqual;
					break;
				default:
					return left;
			}

			parser.AdvanceCursor();

			AstNode::Ptr right{AstRelationalExpression::Parse(parser)};

			if (right == nullptr)
				parser.Error(token._span, "Expected rhs expression");

			left = std::make_unique<AstEqualityExpression>(std::move(left), equalityOperator, std::move(right));
		}
	}

	std::string AstEqualityExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstEqualityExpression, depth,
				{TOSTRING_FIELD_NODE("left", *_left) TOSTRING_FIELD_ENUM("operator", _equalityOperator)
						 TOSTRING_FIELD_NODE("right", *_right)}
		)
	}
}// namespace parsing
