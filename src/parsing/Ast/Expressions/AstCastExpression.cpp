//
// Created by johny on 11/3/23.
//

#include "AstCastExpression.h"
#include "../../Parser.h"
#include "AstUnaryExpression.h"
#include <sstream>

namespace parsing {
	AstNode::Ptr AstCastExpression::Parse(Parser &parser) {
		if (!parser)
			return nullptr;

		int cursor{parser.GetCursor()};

		std::optional<Token> lParen{parser.ConsumeIfTokenIs(TokenType::LeftParenthesis)};
		if (!lParen)
			return AstUnaryExpression::Parse(parser);

		std::optional<AstTypeName> typeName{AstTypeName::Parse(parser)};
		if (!typeName) {
			parser.SetCursor(cursor);
			return AstUnaryExpression::Parse(parser);
		}

		std::optional<Token> rParen{parser.ConsumeIfTokenIs(TokenType::RightParenthesis)};
		if (!rParen) {
			parser.Error(parser.PeekNextToken()._span, "Expected ')'");
		}

		AstNode::Ptr expression{AstCastExpression::Parse(parser)};

		// if the type cast is not followed by an expression, it is not a type cast
		if (!expression) {
			parser.SetCursor(cursor);
			return AstUnaryExpression::Parse(parser);
		}

		SpanOld typeNameSpan{typeName->_span};
		SpanOld expressionSpan{expression->_span};

		AstNode::Ptr castExpression{std::make_unique<AstCastExpression>(std::move(typeName), std::move(expression))};
		castExpression->_span = lParen->_span + typeNameSpan + rParen->_span + expressionSpan;

		return castExpression;
	}

	std::string AstCastExpression::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstCastExpression, depth,
				{TOSTRING_FIELD_NODE("typeName", *_typeName) TOSTRING_FIELD_NODE("expression", *_expression)}
		)
	}
}// namespace parsing
