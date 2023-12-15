//
// Created by johny on 11/3/23.
//

#include "AstMultiplicativeExpression.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../../tokenizer.h"
#include "../../Parser.h"
#include "AstCastExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstMultiplicativeExpression::Parse(Parser &parser) {
		AstNode::Ptr left{AstCastExpression::Parse(parser)};

		if (left == nullptr)
			return nullptr;

		while (true) {
			if (!parser)
				return left;

			const Token &token{parser.PeekNextToken()};

			MultiplicativeOperator multiplicativeOperator;

			switch (token._type) {
				case TokenType::Asterisk:
					multiplicativeOperator = MultiplicativeOperator::Multiply;
					break;
				case TokenType::Divide:
					multiplicativeOperator = MultiplicativeOperator::Divide;
					break;
				case TokenType::Modulo:
					multiplicativeOperator = MultiplicativeOperator::Modulo;
					break;
				default:
					return left;
			}

			parser.AdvanceCursor();

			AstNode::Ptr right{AstCastExpression::Parse(parser)};

			if (right == nullptr)
				parser.Error(token._span, "Expected rhs expression");

			left = std::make_unique<AstMultiplicativeExpression>(
					std::move(left), multiplicativeOperator, std::move(right)
			);
		}
	}

	std::string AstMultiplicativeExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstMultiplicativeExpression, depth,
				{TOSTRING_FIELD_NODE("left", *_left) TOSTRING_FIELD_ENUM("operator", _multiplicativeOperator)
						 TOSTRING_FIELD_NODE("right", *_right)}
		)
	}
}// namespace parsing
