//
// Created by johny on 11/5/23.
//

#include "AstParameterList.h"
#include "../../Parser.h"
#include "AstParameterDeclaration.h"
#include <sstream>

namespace parsing {
	std::optional<AstParameterList> AstParameterList::Parse(Parser &parser) {
		std::vector<AstParameterDeclaration> declarations{};
		std::optional<AstParameterDeclaration> paramDecl{AstParameterDeclaration::Parse(parser)};

		if (!paramDecl)
			return std::nullopt;

		declarations.push_back(std::move(*paramDecl));

		while (true) {
			int currentCursor{parser.GetCursor()};
			std::optional<Token> comma{parser.ConsumeIfTokenIs(TokenType::Comma)};

			if (!comma)
				break;

			paramDecl = AstParameterDeclaration::Parse(parser);
			if (!paramDecl) {
				parser.SetCursor(currentCursor);
				break;
			}

			declarations.push_back(std::move(*paramDecl));
		}

		return AstParameterList{std::move(declarations)};
	}

	std::string AstParameterList::ToString(size_t depth) const {
		TOSTRING_LIST(AstParameterList, depth, {
			for (const auto &paramDeclaration: _paramDeclarations) { TOSTRING_LIST_ITEM_NODE(paramDeclaration) }
		})
	}
}// namespace parsing
