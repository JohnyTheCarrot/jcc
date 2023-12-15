#include "AstLabeledStatement.h"
#include "../../Parser.h"
#include "../Expressions/AstConstantExpression.h"
#include "../Expressions/AstIdentifierExpression.h"
#include "AstStatement.h"
#include <optional>

namespace parsing {
	std::optional<AstLabeledStatement> AstLabeledStatement::Parse(Parser &parser) {
		Span span{};
		std::optional<Token> nextToken{parser.PeekNextToken()};
		Kind kind;
		AstNode::Ptr inner{nullptr};
		if (nextToken->_type == TokenType::KeywordDefault) {
			kind = Kind::Default;
			parser.AdvanceCursor();
		} else if (nextToken->_type == TokenType::KeywordCase) {
			kind = Kind::Case;
			parser.AdvanceCursor();
			inner = AstConstantExpression::Parse(parser);
			if (!inner)
				parser.Error(nextToken->_span, "Exected to be followed by a constant expression");
		} else {
			kind = Kind::Label;
			std::optional<AstIdentifierExpression> identifier{AstIdentifierExpression::Parse(parser)};
			if (!identifier)
				return std::nullopt;
			inner = std::make_unique<AstIdentifierExpression>(*identifier);
		}

		parser.ExpectToken(TokenType::Colon, span);
		AstNode::Ptr statement{AstStatement::Parse(parser)};
		if (!statement)
			parser.Error(nextToken->_span, "Expected to be followed by a statement");

		return AstLabeledStatement{span, kind, std::move(inner), std::move(statement)};
	}

	std::string AstLabeledStatement::ToString(size_t depth) const noexcept {
		TOSTRING_FIELDS(AstLabeledStatement, depth, {
			TOSTRING_FIELD_ENUM("kind", _kind);
			if (_inner)
				TOSTRING_FIELD_NODE("inner", *_inner);
			TOSTRING_FIELD_NODE("statement", *_statement);
		})
	}
}// namespace parsing
