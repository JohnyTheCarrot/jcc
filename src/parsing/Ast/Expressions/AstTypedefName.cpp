#include "AstTypedefName.h"
#include "../../Parser.h"
#include <optional>

using namespace parsing;

std::optional<AstTypedefName> AstTypedefName::Parse(Parser &parser) {
	const int currentCursor{parser.GetCursor()};
	std::optional<Token> ident{parser.ConsumeIfTokenIs(TokenType::Identifier)};
	if (!ident)
		return std::nullopt;

	const std::string identStr{std::get<std::string>(ident->_value)};
	if (parser.typeDefs_.find(identStr) == parser.typeDefs_.end()) {
		//parser.Error(ident->_span, "Unknown type");
		parser.SetCursor(currentCursor);
		return std::nullopt;
	}

	return AstTypedefName{ident->_span, identStr};
}

std::string AstTypedefName::ToString(size_t depth) const {
	TOSTRING_ONE_FIELD_STR(AstTypedefName, depth, this->typeDefName_);
}
