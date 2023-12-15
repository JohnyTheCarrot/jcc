//
// Created by johny on 11/5/23.
//

#include "AstParameterDeclaration.h"
#include "../../Parser.h"
#include <sstream>

namespace parsing {
	std::optional<AstParameterDeclaration> AstParameterDeclaration::Parse(Parser &parser) {
		std::optional<AstDeclarationSpecifiers> declSpecifiers{AstDeclarationSpecifiers::Parse(parser)};
		if (!declSpecifiers)
			return std::nullopt;

		std::optional<AstDeclarator> declarator{AstDeclarator::Parse(parser)};
		if (declarator) {
			return AstParameterDeclaration{std::move(*declSpecifiers), std::move(*declarator)};
		}

		std::optional<AstAbstractDeclarator> abstractDeclarator{AstAbstractDeclarator::Parse(parser)};
		return AstParameterDeclaration{std::move(*declSpecifiers), std::move(abstractDeclarator)};
	}

	std::string AstParameterDeclaration::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstParameterDeclaration, depth, {
			TOSTRING_FIELD_NODE("declarationSpecifiers", _declarationSpecifiers);
			if (std::holds_alternative<AstDeclarator>(_declarator)) {
				TOSTRING_FIELD_NODE("declarator", std::get<AstDeclarator>(_declarator));
			} else if (std::holds_alternative<std::optional<AstAbstractDeclarator>>(_declarator)) {
				const auto &abstractDeclarator{std::get<std::optional<AstAbstractDeclarator>>(_declarator)};
				if (abstractDeclarator.has_value())
					TOSTRING_FIELD_NODE("abstractDeclarator", abstractDeclarator.value());
			}
		})
	}
}// namespace parsing
