//
// Created by johny on 11/3/23.
//

#include "AstShiftExpression.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../Parser.h"
#include "AstAdditiveExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstShiftExpression::Parse(Parser &parser) {
		AstNode::Ptr left{AstAdditiveExpression::Parse(parser)};

		if (left == nullptr)
			return nullptr;

		while (true) {
			if (!parser)
				return left;

			const Token &token{parser.PeekNextToken()};

			ShiftOperator shiftOperator;

			switch (token._type) {
				case TokenType::ShiftLeft:
					shiftOperator = ShiftOperator::ShiftLeft;
					break;
				case TokenType::ShiftRight:
					shiftOperator = ShiftOperator::ShiftRight;
					break;
				default:
					return left;
			}

			parser.AdvanceCursor();

			AstNode::Ptr right{AstAdditiveExpression::Parse(parser)};

			if (right == nullptr)
				parser.Error(token._span, "Expected rhs expression");

			left = std::make_unique<AstShiftExpression>(std::move(left), shiftOperator, std::move(right));
		}
	}

	std::string AstShiftExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstShiftExpression, depth,
				{TOSTRING_FIELD_NODE("left", *_left) TOSTRING_FIELD_ENUM("operator", _shiftOperator)
						 TOSTRING_FIELD_NODE("right", *_right)}
		)
	}
}// namespace parsing
