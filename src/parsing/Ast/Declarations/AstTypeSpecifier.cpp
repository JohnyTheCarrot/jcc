//
// Created by johny on 11/3/23.
//

#include "AstTypeSpecifier.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "../../../misc/tokenizer_old.h"
#include "../../Parser.h"
#include "AstStructOrUnionSpecifier.h"
#include <memory>
#include <optional>
#include <sstream>
#include <variant>

namespace parsing {
	AstNode::Ptr AstTypeSpecifier::Parse(Parser &parser) {
		if (!parser)
			return nullptr;

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
			case TokenType::KeywordUnion: {
				std::optional<AstStructOrUnionSpecifier> spec{AstStructOrUnionSpecifier::Parse(parser)};

				if (!spec.has_value())
					return nullptr;

				return std::make_unique<AstStructOrUnionSpecifier>(std::move(spec.value()));
			}
			case TokenType::KeywordEnum:
			case TokenType::KeywordAtomic:
				TODO()
			case TokenType::Identifier: {
				std::optional<AstTypedefName> &&typedefName{AstTypedefName::Parse(parser)};

				if (!typedefName.has_value()) {
					return nullptr;
				}

				return std::make_unique<AstTypedefName>(std::move(typedefName.value()));
			}
			default:
				return nullptr;
		}

		parser.AdvanceCursor();

		return std::make_unique<AstTypeSpecifier>(token._span, std::move(typeSpecifierType));
	}

	std::string AstTypeSpecifier::ToString(size_t depth) const { TOSTRING_ENUM(AstTypeSpecifier, specifierType_) }
}// namespace parsing
