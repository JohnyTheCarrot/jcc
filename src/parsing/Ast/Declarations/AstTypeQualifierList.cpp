//
// Created by johny on 11/5/23.
//

#include "AstTypeQualifierList.h"
#include <sstream>

namespace parsing {
	std::optional<AstTypeQualifierList> AstTypeQualifierList::Parse(Parser &parser) {
		std::vector<AstTypeQualifier> list;
		SpanOld span;

		if (auto typeQualifier{AstTypeQualifier::Parse(parser)}) {
			span = typeQualifier->_span;
			list.push_back(std::move(*typeQualifier));
		} else
			return std::nullopt;

		while (true) {
			if (auto typeQualifier{AstTypeQualifier::Parse(parser)}) {
				span += typeQualifier->_span;
				list.push_back(std::move(*typeQualifier));
			} else {
				break;
			}
		}

		return AstTypeQualifierList{span, std::move(list)};
	}

	std::string AstTypeQualifierList::ToString(size_t depth) const {
		TOSTRING_LIST(AstTypeQualifierList, depth, {
			for (const auto &typeQualifier: _list) { TOSTRING_LIST_ITEM_NODE(typeQualifier) }
		})
	}
}// namespace parsing
