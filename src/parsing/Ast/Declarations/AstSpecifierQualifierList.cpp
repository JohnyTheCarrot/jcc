//
// Created by johny on 11/3/23.
//

#include "AstSpecifierQualifierList.h"
#include "../../../misc/tokenizer.h"
#include "../../Parser.h"
#include "AstAlignmentSpecifier.h"
#include <sstream>

namespace parsing {
	std::optional<AstSpecifierQualifierList> AstSpecifierQualifierList::Parse(Parser &parser) {
		AstSpecifierQualifierList specifierQualifierListNode{};

		while (true) {
			int parserCursor{parser.GetCursor()};

			AstNode::Ptr typeSpecifier{AstTypeSpecifier::Parse(parser)};
			if (typeSpecifier) {
				specifierQualifierListNode._list.emplace_back(std::move(typeSpecifier));
				continue;
			}

			std::optional<AstTypeQualifier> typeQualifier{AstTypeQualifier::Parse(parser)};
			if (typeQualifier.has_value()) {
				specifierQualifierListNode._list.emplace_back(typeQualifier.value());
				continue;
			}

			std::optional<AstAlignmentSpecifier> alignmentSpecifier{AstAlignmentSpecifier::Parse(parser)};
			if (alignmentSpecifier.has_value()) {
				specifierQualifierListNode._list.emplace_back(std::move(alignmentSpecifier.value()));
				continue;
			}

			parser.SetCursor(parserCursor);
			break;
		}

		if (specifierQualifierListNode._list.empty())
			return std::nullopt;

		return specifierQualifierListNode;
	}

	std::string AstSpecifierQualifierList::ToString(size_t depth) const {
		TOSTRING_LIST(AstSpecifierQualifierList, depth, {
			for (const auto &item: this->_list) {
				if (std::holds_alternative<AstNode::Ptr>(item))
					TOSTRING_LIST_ITEM_NODE(*std::get<AstNode::Ptr>(item))
				else if (std::holds_alternative<AstTypeQualifier>(item))
					TOSTRING_LIST_ITEM_NODE(std::get<AstTypeQualifier>(item))
				else /* alignment specifier */
					TOSTRING_LIST_ITEM_NODE(std::get<AstAlignmentSpecifier>(item))
			}
		})
	}
}// namespace parsing
