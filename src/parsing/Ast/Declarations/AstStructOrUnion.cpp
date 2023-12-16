#include "AstStructOrUnion.h"
#include "../../Parser.h"
#include <optional>

using namespace parsing;

std::optional<AstStructOrUnion> AstStructOrUnion::Parse(Parser &parser) {
	std::optional<Token> nextToken{parser.PeekNextToken()};

	Kind kind;

	switch (nextToken->_type) {
		case TokenType::KeywordStruct:
			kind = Kind::Struct;
			break;
		case TokenType::KeywordUnion:
			kind = Kind::Union;
			break;
		default:
			return std::nullopt;
	}

	parser.AdvanceCursor();

	return AstStructOrUnion{nextToken->_span, kind};
}

std::string AstStructOrUnion::ToString(size_t depth) const { TOSTRING_ENUM(AstStructOrUnion, kind_) }
