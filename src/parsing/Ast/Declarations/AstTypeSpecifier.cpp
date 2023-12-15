//
// Created by johny on 11/3/23.
//

#include "AstTypeSpecifier.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../../tokenizer.h"
#include "../../Parser.h"
#include <optional>
#include <sstream>
#include <variant>

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
			case TokenType::Identifier: {
				const std::optional<AstTypedefName> typedefName{AstTypedefName::Parse(parser)};

				if (!typedefName.has_value())
					return std::nullopt;

				return AstTypeSpecifier{token._span, typedefName.value()};
			}
			default:
				return std::nullopt;
		}

		parser.AdvanceCursor();

		return AstTypeSpecifier{token._span, typeSpecifierType};
	}

	std::string AstTypeSpecifier::ToString(size_t depth) const {
		if (std::holds_alternative<AstTypeSpecifierType>(_specifierType)) {
			TOSTRING_ENUM(AstTypeSpecifier, std::get<AstTypeSpecifierType>(_specifierType))
		} else {
			return std::get<AstTypedefName>(_specifierType).ToString(depth + 1);
		}
	}
}// namespace parsing
