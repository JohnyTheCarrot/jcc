//
// Created by johny on 11/4/23.
//

#include "AstDeclaration.h"
#include "../../Parser.h"
#include "../Expressions/AstIdentifierExpression.h"
#include "AstDeclarationSpecifiers.h"
#include "AstDeclarator.h"
#include "AstInitDeclaratorList.h"
#include "AstStaticAssertDeclaration.h"
#include "AstStorageClassSpecifier.h"
#include <memory>
#include <sstream>
#include <variant>

namespace parsing {
	AstNode::Ptr AstDeclaration::Parse(Parser &parser) {
		std::optional<AstStaticAssertDeclaration> staticAssertDeclaration{AstStaticAssertDeclaration::Parse(parser)};

		if (staticAssertDeclaration)
			return std::make_unique<AstStaticAssertDeclaration>(std::move(*staticAssertDeclaration));

		std::optional<AstDeclarationSpecifiers> declarationSpecifiers{AstDeclarationSpecifiers::Parse(parser)};

		if (!declarationSpecifiers)
			return nullptr;

		std::optional<AstInitDeclaratorList> initDeclaratorList{AstInitDeclaratorList::Parse(parser)};
		parser.ExpectToken(TokenType::Semicolon);

		// assumes typedef keyword is always first in typedef declarations
		const AstDeclarationSpecifiers::DeclarationSpecifier &firstSpecifier{
				declarationSpecifiers->_declarationSpecifiers[0]};
		const bool isFirstSpecifierTypedef{
				std::holds_alternative<AstStorageClassSpecifier>(firstSpecifier) &&
				std::get<AstStorageClassSpecifier>(firstSpecifier)._storageClassSpecifier ==
						AstStorageClassSpecifier::StorageClassSpecifierType::Typedef};

		auto resultDeclPtr{std::make_unique<AstDeclaration>(
				std::make_unique<AstDeclarationSpecifiers>(std::move(*declarationSpecifiers)),
				std::move(initDeclaratorList)
		)};

		if (isFirstSpecifierTypedef) {
			if (!resultDeclPtr->_initDeclaratorList.has_value())
				parser.Error("Expected typedef to contain a declarator list");

			for (const auto &declarator: resultDeclPtr->_initDeclaratorList->_initDeclaratorList) {
				auto astDeclPtr{dynamic_cast<AstDeclarator *>(declarator.get())};
				if (!astDeclPtr)
					parser.Error(declarator->_span, "Expected declarator");

				auto astIdentPtr{dynamic_cast<AstIdentifierExpression *>(astDeclPtr->_directDeclarator.get())};
				if (!astIdentPtr)
					parser.Error(astIdentPtr->_span, "Expected identifier");

				TypeDefinition typeDef{.specifierQualifierList_ = resultDeclPtr->_declarationSpecifiers.get()};
				parser.typeDefs_[astIdentPtr->_ident] = typeDef;
			}
		}


		return std::move(resultDeclPtr);
	}

	std::string AstDeclaration::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstDeclaration, depth, {
			TOSTRING_FIELD_NODE("declarationSpecifiers", *_declarationSpecifiers)
			if (_initDeclaratorList)
				TOSTRING_FIELD_NODE("initDeclaratorList", *_initDeclaratorList)
		})
	}
}// namespace parsing
