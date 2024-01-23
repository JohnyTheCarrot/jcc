//
// Created by johny on 11/12/23.
//

#include "AstFunctionDefinition.h"
#include "../../Parser.h"

namespace parsing {
	std::optional<AstFunctionDefinition> AstFunctionDefinition::Parse(Parser &parser) {
		int cursor{parser.GetCursor()};
		SpanOld span{};

		std::optional<AstDeclarationSpecifiers> specifiers{AstDeclarationSpecifiers::Parse(parser)};
		if (!specifiers)
			return std::nullopt;

		span += specifiers->_span;

		std::optional<AstDeclarator> declarator{AstDeclarator::Parse(parser)};
		if (!declarator) {
			parser.SetCursor(cursor);
			return std::nullopt;
		}

		span += declarator->_span;

		if (!parser)
			parser.Error("Unexpected end of input");

		const Token &nextToken{parser.PeekNextToken()};

		if (nextToken._type == TokenType::Semicolon || nextToken._type == TokenType::Comma ||
			nextToken._type == TokenType::Assign) {
			// Function declaration
			parser.SetCursor(cursor);
			return std::nullopt;
		}

		std::optional<AstCompoundStatement> compoundStatement{AstCompoundStatement::Parse(parser)};
		if (!compoundStatement)
			parser.Error("Expected to be followed by compound statement");

		span += compoundStatement->_span;

		return AstFunctionDefinition{
				span, std::move(*specifiers), std::move(*declarator), std::move(*compoundStatement)};
	}

	std::string AstFunctionDefinition::ToString(size_t depth) const {
		TOSTRING_FIELDS(
				AstFunctionDefinition, depth,
				{TOSTRING_FIELD_NODE("declarationSpecifiers", _declarationSpecifiers)
						 TOSTRING_FIELD_NODE("declarator", _declarator)
								 TOSTRING_FIELD_NODE("compoundStatement", _compoundStatement)}
		)
	}
}// namespace parsing
