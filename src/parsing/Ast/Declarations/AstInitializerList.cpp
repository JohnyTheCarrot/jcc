//
// Created by johny on 11/5/23.
//

#include "AstInitializerList.h"
#include "../../Parser.h"
#include "AstInitializer.h"
#include <sstream>

namespace parsing {
	std::optional<AstInitializerList> AstInitializerList::Parse(Parser &parser) {
		std::vector<InitializerListItem> initializerListItems{};
		Span span{};
		bool isFirstIteration{true};
		int cursor{parser.GetCursor()};

		while (true) {
			std::optional<AstDesignation> designation{AstDesignation::Parse(parser)};
			AstNode::Ptr initializer{AstInitializer::Parse(parser)};

			if (!initializer) {
				if (designation)
					parser.Error(designation->_span, "Expected to be followed by initializer");

				if (isFirstIteration)
					return std::nullopt;

				parser.SetCursor(cursor);
				break;
			}

			isFirstIteration = false;

			if (designation)
				designation->_span;
			span += initializer->_span;
			initializerListItems.emplace_back(std::move(designation), std::move(initializer));

			cursor = parser.GetCursor();
			parser.AdvanceIfTokenIs(TokenType::Comma);
		}

		return AstInitializerList{span, std::move(initializerListItems)};
	}

	std::string AstInitializerList::ToString(size_t depth) const {TOSTRING_LIST(
			AstInitializerList, depth,
			{
				for (const auto &listItem: _listItems) { TOSTRING_LIST_ITEM_NODE(listItem) }
			}
	)}

	std::string AstInitializerList::InitializerListItem::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstInitializerList::InitializerListItem, depth, {
			if (_designation)
				TOSTRING_FIELD_NODE("designation", *_designation)

			TOSTRING_FIELD_NODE("initializer", *_initializer)
		})
	}
}// namespace parsing
