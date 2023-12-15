//
// Created by johny on 11/4/23.
//

#include "AstRelationalExpression.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../Parser.h"
#include "AstShiftExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstRelationalExpression::Parse(Parser &parser) {
		AstNode::Ptr left{AstShiftExpression::Parse(parser)};

		if (left == nullptr)
			return nullptr;

		while (true) {
			if (!parser)
				return left;

			const Token &token{parser.PeekNextToken()};

			RelationalOperator relationalOperator;

			switch (token._type) {
				case TokenType::LessThan:
					relationalOperator = RelationalOperator::LessThan;
					break;
				case TokenType::GreaterThan:
					relationalOperator = RelationalOperator::GreaterThan;
					break;
				case TokenType::LessThanOrEqual:
					relationalOperator = RelationalOperator::LessThanOrEqual;
					break;
				case TokenType::GreaterThanOrEqual:
					relationalOperator = RelationalOperator::GreaterThanOrEqual;
					break;
				default:
					return left;
			}

			parser.AdvanceCursor();

			AstNode::Ptr right{AstShiftExpression::Parse(parser)};

			if (right == nullptr)
				parser.Error(token._span, "Expected rhs expression");

			left = std::make_unique<AstRelationalExpression>(std::move(left), relationalOperator, std::move(right));
		}
	}

	std::string AstRelationalExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstRelationalExpression, depth,
				{TOSTRING_FIELD_NODE("left", *_left) TOSTRING_FIELD_ENUM("operator", _relationalOperator)
						 TOSTRING_FIELD_NODE("right", *_right)}
		)
	}
}// namespace parsing
