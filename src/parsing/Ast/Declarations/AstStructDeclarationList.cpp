#include "AstStructDeclarationList.h"
#include "AstStructDeclaration.h"
#include <optional>

using namespace parsing;

std::optional<AstStructDeclarationList> AstStructDeclarationList::Parse(Parser &parser) {
	Span span{};
	StructDeclarationList list{};

	while (true) {
		StructDeclaration decl{AstStructDeclaration::Parse(parser)};
		if (!decl) {
			if (list.empty())
				return std::nullopt;

			return AstStructDeclarationList{span, std::move(list)};
		}

		span += decl->_span;
		list.push_back(std::move(decl));
	}
}

std::string AstStructDeclarationList::ToString(size_t depth) const {
	TOSTRING_LIST(AstStructDeclarationList, depth, {
		for (const auto &decl: list_) TOSTRING_LIST_ITEM_NODE(*decl);
	});
}
