#include "AstJumpStatement.h"
#include "../../Parser.h"
#include "../Expressions/AstExpression.h"
#include "../Expressions/AstIdentifierExpression.h"
#include <memory>
#include <optional>

namespace parsing {
	std::optional<AstJumpStatement> AstJumpStatement::Parse(Parser &parser) noexcept {
		Span span{};
		const std::optional<Token> nextToken{parser.PeekNextToken()};
		if (!nextToken)
			return std::nullopt;

		Kind kind;

		switch (nextToken->_type) {
			case TokenType::KeywordGoto:
				kind = Kind::Goto;
				break;
			case TokenType::KeywordContinue:
				kind = Kind::Continue;
				break;
			case TokenType::KeywordBreak:
				kind = Kind::Break;
				break;
			case TokenType::KeywordReturn:
				kind = Kind::Return;
				break;
			default:
				return std::nullopt;
		}

		span += nextToken->_span;
		parser.AdvanceCursor();
		AstNode::Ptr inner{nullptr};

		if (kind == Kind::Goto) {
			const std::optional<AstIdentifierExpression> identifierExpr{AstIdentifierExpression::Parse(parser)};
			if (!identifierExpr)
				parser.Error(nextToken->_span, "Expected to be followed by identifier");
			inner = std::make_unique<AstIdentifierExpression>(*identifierExpr);
		} else if (kind == Kind::Return) {
			inner = AstExpression::Parse(parser);
		}

		parser.ExpectToken(TokenType::Semicolon, span);
		return AstJumpStatement{span, kind, std::move(inner)};
	}

	std::string AstJumpStatement::ToString(size_t depth) const noexcept {
		TOSTRING_FIELDS(AstJumpStatement, depth, {
			TOSTRING_FIELD_ENUM("kind", _kind);
			if (_inner)
				TOSTRING_FIELD_NODE("inner", *_inner);
		})
	}
}// namespace parsing
