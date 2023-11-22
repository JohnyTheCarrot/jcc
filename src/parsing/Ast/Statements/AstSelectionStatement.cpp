//
// Created by johny on 11/21/23.
//

#include "AstSelectionStatement.h"
#include "../../Parser.h"
#include "../Expressions/AstExpression.h"
#include "AstStatement.h"
#include <optional>

namespace parsing {
	std::optional<AstSelectionStatement> AstSelectionStatement::Parse(Parser &parser) noexcept {
		Span span{};
		const std::optional<Token> keywordIf{parser.ConsumeIfTokenIs(TokenType::KeywordIf)};
		if (!keywordIf) {
			const std::optional<Token> keywordSwitch{parser.ConsumeIfTokenIs(TokenType::KeywordSwitch)};
			if (!keywordSwitch) return std::nullopt;
			span += keywordSwitch->_span;

			const Token &leftParen{parser.ExpectToken(TokenType::LeftParenthesis, span)};
			AstNode::Ptr expression{AstExpression::Parse(parser)};
			if (!expression) parser.Error(leftParen._span, "Expected to be followed by expression");
			span += expression->_span;

			const Token &rightParen{parser.ExpectToken(TokenType::RightParenthesis, span)};
			AstNode::Ptr statement{AstStatement::Parse(parser)};
			if (!statement) parser.Error(rightParen._span, "Expected to be followed by statement");
			span += statement->_span;

			return AstSelectionStatement{span, std::move(expression), std::move(statement), nullptr, Kind::Switch};
		};

		span = keywordIf->_span;

		parser.ExpectToken(TokenType::LeftParenthesis, span);

		AstNode::Ptr condition{AstExpression::Parse(parser)};
		if (!condition) parser.Error("If statement is missing _condition");

		const Token &rightParen{parser.ExpectToken(TokenType::RightParenthesis, span)};

		AstNode::Ptr ifBody{AstStatement::Parse(parser)};
		if (!ifBody) parser.Error(rightParen._span, "If statement is missing body");

		const std::optional<Token> keywordElse{parser.ConsumeIfTokenIs(TokenType::KeywordElse)};
		AstNode::Ptr elseBody{nullptr};
		if (keywordElse) {
			elseBody = AstStatement::Parse(parser);
			if (!elseBody) parser.Error(keywordElse->_span, "Else statement is missing body");
			span += elseBody->_span + keywordElse->_span;
		}

		return AstSelectionStatement{span, std::move(condition), std::move(ifBody), std::move(elseBody), Kind::If};
	}

	std::string AstSelectionStatement::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstSelectionStatement, depth, {
			TOSTRING_FIELD_NODE("condition", *_condition)
			TOSTRING_FIELD_NODE("body", *_body)
			if (_elseBody) TOSTRING_FIELD_NODE("elseBody", *_elseBody);
			TOSTRING_FIELD_ENUM("kind", _kind);
		})
	}
}// namespace parsing
