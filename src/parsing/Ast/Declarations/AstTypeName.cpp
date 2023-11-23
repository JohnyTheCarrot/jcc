//
// Created by johny on 11/3/23.
//

#include "AstTypeName.h"
#include "../../../tokenizer.h"
#include "../../Parser.h"
#include "AstAbstractDeclarator.h"
#include <sstream>

namespace parsing {
	std::optional<AstTypeName> AstTypeName::Parse(Parser &parser) {
		std::optional<AstSpecifierQualifierList> list{AstSpecifierQualifierList::Parse(parser)};

		if (!list.has_value()) { return std::nullopt; }

		std::optional<AstAbstractDeclarator> abstractDeclarator{AstAbstractDeclarator::Parse(parser)};

		return AstTypeName{std::move(*list), std::move(abstractDeclarator)};
	}

	std::string AstTypeName::ToString(size_t depth) const {
		TOSTRING_ONE_FIELD_NODE(AstTypeName, depth, _specifierQualifierList)
	}
}// namespace parsing
