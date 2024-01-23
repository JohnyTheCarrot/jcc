#include "AstStructDeclaration.h"
#include "../../Parser.h"
#include "AstSpecifierQualifierList.h"
#include "AstStaticAssertDeclaration.h"
#include "AstStructDeclaratorList.h"
#include <memory>

using namespace parsing;

AstNode::Ptr AstStructDeclaration::Parse(Parser &parser) {
	std::optional<AstStaticAssertDeclaration> staticAssertDecl{AstStaticAssertDeclaration::Parse(parser)};

	if (staticAssertDecl.has_value())
		return std::make_unique<AstStaticAssertDeclaration>(std::move(staticAssertDecl.value()));

	SpanOld span{};
	std::optional<AstSpecifierQualifierList> specifierQualifierList{AstSpecifierQualifierList::Parse(parser)};

	if (!specifierQualifierList.has_value())
		return nullptr;

	span += specifierQualifierList->_span;

	std::optional<AstStructDeclaratorList> declaratorList{AstStructDeclaratorList::Parse(parser)};

	if (declaratorList.has_value())
		span += declaratorList->_span;

	parser.ExpectToken(TokenType::Semicolon, span);

	return std::make_unique<AstStructDeclaration>(
			span, std::move(specifierQualifierList.value()), std::move(declaratorList)
	);
}

std::string AstStructDeclaration::ToString(size_t depth) const {
	TOSTRING_FIELDS(AstStructDeclaration, depth, {
		TOSTRING_FIELD_NODE("specifierQualifierList", specifierQualifierList_);
		if (structDeclaratorList_)
			TOSTRING_FIELD_NODE("structDeclaratorList", *structDeclaratorList_);
	});
}
