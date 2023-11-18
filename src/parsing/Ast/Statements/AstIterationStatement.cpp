//
// Created by johny on 11/12/23.
//

#include "AstIterationStatement.h"
#include "../../Parser.h"
#include "../Declarations/AstDeclaration.h"
#include "../Expressions/AstExpression.h"
#include "AstStatement.h"

namespace parsing {
	std::optional<AstWhileIterationStatement> AstWhileIterationStatement::Parse(Parser &parser) {
		Span span{};

		const std::optional<Token> tokenDo{parser.ConsumeIfTokenIs(TokenType::KeywordDo)};
		if (tokenDo) {
			span += tokenDo->_span;

			AstNode::Ptr statement{AstStatement::Parse(parser)};
			if (!statement) parser.Error(tokenDo->_span, "Expected statement");

			const std::optional<Token> tokenWhile{parser.ConsumeIfTokenIs(TokenType::KeywordWhile)};
			if (!tokenWhile) return std::nullopt;

			span += tokenWhile->_span;

			const Token &lParen{parser.ExpectToken(TokenType::LeftParenthesis, span)};

			AstNode::Ptr expression{AstExpression::Parse(parser)};
			if (!expression) parser.Error(lParen._span, "Expected expression");

			parser.ExpectToken(TokenType::RightParenthesis, span);
			parser.ExpectToken(TokenType::Semicolon, span);

			return AstWhileIterationStatement{span, std::move(expression), std::move(statement), true};
		}

		const std::optional<Token> tokenWhile{parser.ConsumeIfTokenIs(TokenType::KeywordWhile)};
		if (!tokenWhile) return std::nullopt;

		span += tokenWhile->_span;

		const Token &lParen{parser.ExpectToken(TokenType::LeftParenthesis, span)};

		AstNode::Ptr expression{AstExpression::Parse(parser)};
		if (!expression) parser.Error(lParen._span, "Expected expression");

		const Token &rParen{parser.ExpectToken(TokenType::RightParenthesis, span)};

		AstNode::Ptr statement{AstStatement::Parse(parser)};
		if (!statement) parser.Error(rParen._span, "Expected statement");

		return AstWhileIterationStatement{span, std::move(expression), std::move(statement), false};
	}

	std::string AstWhileIterationStatement::ToString(size_t depth) const {TOSTRING_FIELDS(
			AstWhileIterationStatement, depth,
			{TOSTRING_FIELD_NODE("condition", *_condition) TOSTRING_FIELD_NODE("statement", *_statement)
					 TOSTRING_FIELD_BOOL("isDoWhile", _isDoWhile)}
	)}

	std::string AstForIterationStatement::ToString(size_t depth) const {TOSTRING_FIELDS(
			AstForIterationStatement, depth,
			{
				if (_setUp) TOSTRING_FIELD_NODE("setUp", *_setUp)
				if (_check) TOSTRING_FIELD_NODE("check", *_check)
				if (_after) TOSTRING_FIELD_NODE("after", *_check)
			}
	)}

	std::optional<AstForIterationStatement> AstForIterationStatement::Parse(Parser &parser) {
		Span span{};

		const std::optional<Token> forToken{parser.ConsumeIfTokenIs(TokenType::KeywordFor)};
		if (!forToken) return std::nullopt;

		span += forToken->_span;

		parser.ExpectToken(TokenType::LeftParenthesis, span);

		AstNode::Ptr setUp{AstDeclaration::Parse(parser)};
		if (!setUp) {
			setUp = AstExpression::Parse(parser);
			parser.ExpectToken(TokenType::Semicolon, span);
		}

		AstNode::Ptr check{AstExpression::Parse(parser)};
		parser.ExpectToken(TokenType::Semicolon, span);

		AstNode::Ptr after{AstExpression::Parse(parser)};

		const Token &rParen{parser.ExpectToken(TokenType::RightParenthesis)};

		const AstNode::Ptr statement{AstStatement::Parse(parser)};
		if (!statement) parser.Error(rParen._span, "Expected to be followed by statement");

		return AstForIterationStatement{span, std::move(setUp), std::move(check), std::move(after)};
	}

	std::string AstIterationStatement::ToString(size_t depth) const {NOT_APPLICABLE()}

	AstNode::Ptr AstIterationStatement::Parse(Parser &parser) {
		std::optional<AstWhileIterationStatement> whileIterationStatement{AstWhileIterationStatement::Parse(parser)};
		if (whileIterationStatement)
			return std::make_unique<AstWhileIterationStatement>(std::move(*whileIterationStatement));

		std::optional<AstForIterationStatement> forIterationStatement{AstForIterationStatement::Parse(parser)};
		if (forIterationStatement) return std::make_unique<AstForIterationStatement>(std::move(*forIterationStatement));

		return nullptr;
	}
}// namespace parsing
