#include "AstStructDeclaratorList.h"
#include "../../Parser.h"
#include "AstStructDeclarator.h"
#include <memory>

using namespace parsing;

std::optional<AstStructDeclaratorList> AstStructDeclaratorList::Parse(Parser &parser) {
	Span span{};

	StructDeclaratorPtrList list{};

	while (true) {
		StructDeclaratorPtr declaratorPtr{AstStructDeclarator::Parse(parser)};

		if (!declaratorPtr) {
			if (list.empty())
				return std::nullopt;

			parser.Error("Expected struct declarator");
		}

		span += declaratorPtr->_span;
		list.push_back(std::move(declaratorPtr));

		const std::optional<Token> nextToken{parser.ConsumeIfTokenIs(TokenType::Comma)};

		if (!nextToken.has_value()) {
			return AstStructDeclaratorList{span, std::move(list)};
		}
		span += nextToken->_span;
	}
}

std::string AstStructDeclaratorList::ToString(size_t depth) const {
	TOSTRING_LIST(AstStructDeclaratorList, depth, {
		for (const auto &declarator: declarators_) { TOSTRING_LIST_ITEM_NODE(*declarator) }
	})
}
