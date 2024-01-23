#include "AstStructDeclarator.h"
#include "../../Parser.h"
#include "../Expressions/AstConstantExpression.h"
#include "AstDeclarator.h"
#include <memory>
#include <optional>

using namespace parsing;

AstNode::Ptr AstStructDeclarator::Parse(Parser &parser) {
	std::optional<AstDeclarator> declarator{AstDeclarator::Parse(parser)};

	std::optional<Token> colonToken{parser.ConsumeIfTokenIs(TokenType::Colon)};

	if (!colonToken.has_value()) {
		if (declarator.has_value()) {
			return std::make_unique<AstDeclarator>(std::move(declarator.value()));
		}

		return nullptr;
	}

	AstNode::Ptr constantExpression{AstConstantExpression::Parse(parser)};
	if (!constantExpression)
		parser.Error("Expected to be followed by constant expression");

	SpanOld span{};
	if (declarator)
		span += declarator->_span;

	span += colonToken->_span + constantExpression->_span;

	return std::make_unique<AstStructDeclarator>(span, std::move(declarator), std::move(constantExpression));
}

std::string AstStructDeclarator::ToString(size_t depth) const {
	TOSTRING_FIELDS(AstStructDeclarator, depth, {
		if (declarator_)
			TOSTRING_FIELD_NODE("declarator", *declarator_);

		TOSTRING_FIELD_NODE("constantExpression", *constantExpression_);
	})
}
