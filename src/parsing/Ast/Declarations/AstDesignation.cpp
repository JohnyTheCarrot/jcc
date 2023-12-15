//
// Created by johny on 11/5/23.
//

#include "AstDesignation.h"
#include "../../Parser.h"
#include <sstream>

namespace parsing {
	std::optional<AstDesignation> AstDesignation::Parse(Parser &parser) {
		std::optional<AstDesignatorList> designatorList{AstDesignatorList::Parse(parser)};
		if (!designatorList)
			return std::nullopt;

		const Token &equalsToken{parser.ExpectToken(TokenType::Assign)};

		return AstDesignation{designatorList->_span + equalsToken._span, std::move(*designatorList)};
	}

	std::string AstDesignation::ToString(size_t depth) const {
		TOSTRING_FIELDS(AstDesignation, depth, {TOSTRING_FIELD_NODE("designatorList", _designatorList)})
	}
}// namespace parsing
