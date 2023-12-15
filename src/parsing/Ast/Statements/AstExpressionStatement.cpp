//
// Created by johny on 11/12/23.
//

#include "AstExpressionStatement.h"
#include "../../Parser.h"
#include "../Expressions/AstExpression.h"

namespace parsing {
	std::optional<AstExpressionStatement> AstExpressionStatement::Parse(Parser &parser) {
		Span span{};
		int cursor{parser.GetCursor()};

		AstNode::Ptr optionalExpression{AstExpression::Parse(parser)};
		if (!parser)
			return std::nullopt;

		const Token &nextToken{parser.PeekNextToken()};

		if (nextToken._type != TokenType::Semicolon) {
			parser.SetCursor(cursor);
			return std::nullopt;
		}
		parser.AdvanceCursor();

		if (optionalExpression)
			span += optionalExpression->_span;

		span += nextToken._span;
		return AstExpressionStatement(span, std::move(optionalExpression));
	}

	std::string AstExpressionStatement::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstExpressionStatement, depth, {
			if (_expression)
				TOSTRING_FIELD_NODE("expression", *_expression)
		})
	}
}// namespace parsing
