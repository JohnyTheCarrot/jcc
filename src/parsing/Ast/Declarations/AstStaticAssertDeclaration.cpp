//
// Created by johny on 11/4/23.
//

#include "AstStaticAssertDeclaration.h"
#include "../../Parser.h"
#include "../Expressions/AstConstantExpression.h"
#include <sstream>

namespace parsing {
	std::optional<AstStaticAssertDeclaration> AstStaticAssertDeclaration::Parse(Parser &parser) {
		std::optional<Token> keyword{parser.ConsumeIfTokenIs(TokenType::KeywordStaticAssert)};

		if (!keyword)
			return std::nullopt;

		parser.ExpectToken(TokenType::LeftParenthesis);

		AstNode::Ptr constantExpression{AstConstantExpression::Parse(parser)};

		if (!constantExpression) {
			parser.Error(keyword->_span, "Expected constant expression");
		}

		parser.ExpectToken(TokenType::Comma);

		std::optional<AstStringLiteralExpression> stringLiteral{AstStringLiteralExpression::Parse(parser)};

		if (!stringLiteral) {
			parser.Error(keyword->_span, "Expected string literal");
		}

		parser.ExpectToken(TokenType::RightParenthesis);
		parser.ExpectToken(TokenType::Semicolon);

		return AstStaticAssertDeclaration{std::move(constantExpression), *stringLiteral};
	}

	std::string AstStaticAssertDeclaration::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstStaticAssertDeclaration, depth,
				{TOSTRING_FIELD_NODE("constantExpression", *_constantExpression)
						 TOSTRING_FIELD_NODE("message", _message)}
		)
	}
}// namespace parsing
