//
// Created by johny on 11/3/23.
//

#include "AstAdditiveExpression.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../Parser.h"
#include "AstMultiplicativeExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstAdditiveExpression::Parse(Parser &parser) {
		AstNode::Ptr left{AstMultiplicativeExpression::Parse(parser)};

		if (left == nullptr)
			return nullptr;

		while (true) {
			if (!parser)
				return left;

			const Token &token{parser.PeekNextToken()};

			AdditiveOperator additiveOperator;

			switch (token._type) {
				case TokenType::Plus:
					additiveOperator = AdditiveOperator::Plus;
					break;
				case TokenType::Minus:
					additiveOperator = AdditiveOperator::Minus;
					break;
				default:
					return left;
			}

			parser.AdvanceCursor();

			AstNode::Ptr right{AstMultiplicativeExpression::Parse(parser)};

			if (right == nullptr)
				parser.Error(token._span, "Expected rhs expression");

			left = std::make_unique<AstAdditiveExpression>(std::move(left), additiveOperator, std::move(right));
		}
	}

	std::string AstAdditiveExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstAdditiveExpression, depth,
				{TOSTRING_FIELD_NODE("left", *_left) TOSTRING_FIELD_ENUM("operator", _additiveOperator)
						 TOSTRING_FIELD_NODE("right", *_right)}
		)
	}
}// namespace parsing
