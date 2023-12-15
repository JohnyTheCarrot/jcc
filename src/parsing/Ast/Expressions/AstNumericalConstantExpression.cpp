//
// Created by johny on 10/19/23.
//

#include "AstNumericalConstantExpression.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../Parser.h"
#include <sstream>

namespace parsing {
	std::optional<AstNumericalConstantExpression> AstNumericalConstantExpression::Parse(Parser &parser) {
		Token token{parser.PeekNextToken()};

		switch (token._type) {
			case TokenType::IntegerLiteral:
				parser.AdvanceCursor();
				return AstNumericalConstantExpression(std::get<IntegerLiteralTokenValue>(token._value));
			case TokenType::DoubleLiteral:
				assert(false && "TODO");
			default:
				return std::nullopt;
		}
	}

	std::string AstNumericalConstantExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstNumericalConstantExpression, depth, {
			if (std::holds_alternative<double>(_value))
				TOSTRING_FIELD_DIRECT("double", std::get<double>(_value))
			else if (std::holds_alternative<IntegerLiteralTokenValue>(_value)) {
				IntegerLiteralTokenValue value = std::get<IntegerLiteralTokenValue>(_value);

				TOSTRING_FIELD_BOOL("isUnsigned", value.isUnsigned)
				TOSTRING_FIELD_ENUM("type", value.type)
				TOSTRING_FIELD_DIRECT("value", value.value)
			}
		})
	}
}// namespace parsing
