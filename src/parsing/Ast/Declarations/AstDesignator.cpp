//
// Created by johny on 11/5/23.
//

#include "AstDesignator.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../Parser.h"
#include "../Expressions/AstConstantExpression.h"
#include "../Expressions/AstIdentifierExpression.h"
#include <sstream>

namespace parsing {
	std::optional<AstDesignator> AstDesignator::Parse(Parser &parser) {
		if (!parser)
			return std::nullopt;

		const Token &token{parser.PeekNextToken()};

		if (token._type == TokenType::LeftBracket) {
			parser.AdvanceCursor();

			AstNode::Ptr constantExpression{AstConstantExpression::Parse(parser)};
			if (!constantExpression)
				parser.Error(token._span, "Expected to be followed by constant expression");

			const Token &rightBracket{parser.ExpectToken(TokenType::RightBracket)};

			return AstDesignator{
					token._span + constantExpression->_span + rightBracket._span, Kind::ArrayIndex,
					std::move(constantExpression)
			};
		}

		if (token._type == TokenType::Dot) {
			parser.AdvanceCursor();

			std::optional<AstIdentifierExpression> ident{AstIdentifierExpression::Parse(parser)};
			if (!ident)
				parser.Error(token._span, "Expected to be followed by identifier");

			return AstDesignator{
					token._span + ident->_span, Kind::StructOrUnionMember,
					std::make_unique<AstIdentifierExpression>(std::move(*ident))
			};
		}

		return std::nullopt;
	}

	std::string AstDesignator::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstDesignator, depth, {TOSTRING_FIELD_ENUM("kind", _kind) TOSTRING_FIELD_NODE("value", *_value)}
		)
	}
}// namespace parsing
