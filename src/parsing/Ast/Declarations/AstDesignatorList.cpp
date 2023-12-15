//
// Created by johny on 11/5/23.
//

#include "AstDesignatorList.h"
#include "../../Parser.h"
#include <sstream>

namespace parsing {
	std::optional<AstDesignatorList> AstDesignatorList::Parse(Parser &parser) {
		std::vector<AstDesignator> designators{};
		Span span;

		std::optional<AstDesignator> designator{AstDesignator::Parse(parser)};
		if (!designator)
			return std::nullopt;

		span += designator->_span;
		designators.push_back(std::move(*designator));

		while (true) {
			designator = AstDesignator::Parse(parser);
			if (!designator) {
				return AstDesignatorList{span, std::move(designators)};
			}

			span += designator->_span;
			designators.push_back(std::move(*designator));
		}
	}

	std::string AstDesignatorList::ToString(size_t depth) const {
		//        std::stringstream ss;
		//        std::string tabs{ Indent(depth) };
		//        std::string tabsChildren{ Indent(depth + 1) };
		//
		//        ss << "DesignatorList([" << std::endl;
		//
		//        for (const auto &designator: _designators) {
		//            ss << tabsChildren << designator.ToString(depth + 1) << std::endl;
		//        }
		//
		//        ss << tabs << "])";
		//
		//        return ss.str();
		TOSTRING_LIST(AstDesignatorList, depth, {
			for (const auto &designator: _designators) { TOSTRING_LIST_ITEM_NODE(designator) }
		})
	}
}// namespace parsing
