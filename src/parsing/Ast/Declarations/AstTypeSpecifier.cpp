//
// Created by johny on 11/3/23.
//

#include "AstTypeSpecifier.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../../tokenizer.h"
#include "../../Parser.h"
#include <sstream>

namespace parsing {
	std::optional<AstTypeSpecifier> AstTypeSpecifier::Parse(Parser &parser) {
		if (!parser)
			return std::nullopt;

		const Token &token{parser.PeekNextToken()};

		AstTypeSpecifierType typeSpecifierType;

		switch (token._type) {
			case TokenType::KeywordVoid:
				typeSpecifierType = AstTypeSpecifierType::Void;
				break;
			case TokenType::KeywordChar:
				typeSpecifierType = AstTypeSpecifierType::Char;
				break;
			case TokenType::KeywordShort:
				typeSpecifierType = AstTypeSpecifierType::Short;
				break;
			case TokenType::KeywordInt:
				typeSpecifierType = AstTypeSpecifierType::Int;
				break;
			case TokenType::KeywordLong:
				typeSpecifierType = AstTypeSpecifierType::Long;
				break;
			case TokenType::KeywordFloat:
				typeSpecifierType = AstTypeSpecifierType::Float;
				break;
			case TokenType::KeywordDouble:
				typeSpecifierType = AstTypeSpecifierType::Double;
				break;
			case TokenType::KeywordSigned:
				typeSpecifierType = AstTypeSpecifierType::Signed;
				break;
			case TokenType::KeywordUnsigned:
				typeSpecifierType = AstTypeSpecifierType::Unsigned;
				break;
			case TokenType::KeywordBool:
				typeSpecifierType = AstTypeSpecifierType::Bool;
				break;
			case TokenType::KeywordComplex:
				typeSpecifierType = AstTypeSpecifierType::Complex;
				break;
			case TokenType::KeywordStruct:
			case TokenType::KeywordUnion:
			case TokenType::KeywordEnum:
			case TokenType::KeywordAtomic:
				TODO()
			default:
				return std::nullopt;
		}

		parser.AdvanceCursor();

		return AstTypeSpecifier{token._span, typeSpecifierType};
	}

	std::string AstTypeSpecifier::ToString(size_t depth) const { TOSTRING_ENUM(AstTypeSpecifier, _specifierType) }
}// namespace parsing
