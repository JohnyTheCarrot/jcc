#include "AstStructOrUnionSpecifier.h"
#include "../../Parser.h"
#include "AstStructDeclarationList.h"
#include "AstStructOrUnion.h"
#include <optional>

using namespace parsing;

std::optional<AstStructOrUnionSpecifier> AstStructOrUnionSpecifier::Parse(Parser &parser) {
	std::optional<AstStructOrUnion> structOrUnion{AstStructOrUnion::Parse(parser)};

	if (!structOrUnion)
		return std::nullopt;

	SpanOld span{structOrUnion->_span};

	std::optional<AstIdentifierExpression> identExpr{AstIdentifierExpression::Parse(parser)};

	std::optional<Token> lBrace{parser.ConsumeIfTokenIs(TokenType::LeftBrace)};

	if (!lBrace.has_value() && !identExpr.has_value()) {
		parser.Error("Expected either an identifier or a left brace");
	}

	if (identExpr.has_value()) {
		span += identExpr->_span;
	}

	if (!lBrace.has_value()) {
		return AstStructOrUnionSpecifier{
				span, std::move(structOrUnion.value()), std::move(identExpr.value()), std::nullopt};
	}

	span += lBrace->_span;

	std::optional<AstStructDeclarationList> declList{AstStructDeclarationList::Parse(parser)};
	if (!declList.has_value()) {
		parser.Error(structOrUnion->_span, "Struct must have at least one declaration when using braces");
	}
	span += declList->_span;

	parser.ExpectToken(TokenType::RightBrace, span);

	return AstStructOrUnionSpecifier{span, std::move(structOrUnion.value()), std::move(identExpr), std::move(declList)};
}

std::string AstStructOrUnionSpecifier::ToString(size_t depth) const {
	TOSTRING_FIELDS(AstStructOrUnionSpecifier, depth, {
		TOSTRING_FIELD_NODE("structOrUnion", structOrUnion_);
		if (identifierExpression_.has_value())
			TOSTRING_FIELD_NODE("identifierExpression", *identifierExpression_);

		if (declList_.has_value())
			TOSTRING_FIELD_NODE("structDeclarationList", *declList_);
	});
}
