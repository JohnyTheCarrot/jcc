#include "Preprocessor.h"
#include "CompilerDatatypes.h"
#include "reporting.h"
#include <magic_enum/magic_enum.hpp>
#include <regex>

std::tuple<Preprocessor::Punctuator, Preprocessor::Punctuator>
CollapsePartialPunctuator(Preprocessor::Punctuator punctuator) {
	using Punctuator = Preprocessor::Punctuator;

	switch (punctuator) {
		case Punctuator::PartialLeftShift:
			return {Punctuator::LessThanLessThan, Punctuator::None};
		case Punctuator::PartialRightShift:
			return {Punctuator::GreaterThanGreaterThan, Punctuator::None};
		case Punctuator::PartialHashHashDigraph:
			return {Punctuator::Hash, Punctuator::None};
		case Punctuator::HashHashDigraphFailure:
		case Punctuator::PartialHashHashDigraphStage2:
			return {Punctuator::Hash, Punctuator::Percent};
		case Punctuator::PartialEllipsis:
			return {Punctuator::Dot, Punctuator::Dot};
		default:
			return {punctuator, Punctuator::None};
	}
}

std::tuple<Preprocessor::Punctuator, Preprocessor::Punctuator> CollapseFailures(Preprocessor::Punctuator punctuator) {
	using Punctuator = Preprocessor::Punctuator;

	switch (punctuator) {
		case Punctuator::HashHashDigraphFailure:
			return {Punctuator::Hash, Punctuator::Percent};
		case Punctuator::EllipsisFailure:
			return {Punctuator::Dot, Punctuator::Dot};
		default:
			return {punctuator, Punctuator::None};
	}
}

Preprocessor::Punctuator
HandlePreviousPunctuator(Preprocessor::StringConstIter &current, Preprocessor::Punctuator prev) {
	using Punctuator = Preprocessor::Punctuator;
	Punctuator result;

	switch (prev) {
		case Punctuator::Minus:
			switch (*current) {
				case '-':
					result = Punctuator::MinusMinus;
					break;
				case '>':
					result = Punctuator::Arrow;
					break;
				case '=':
					result = Punctuator::MinusEqual;
					break;
				default:
					return Punctuator::Minus;
			}
			break;
		case Punctuator::Plus:
			switch (*current) {
				case '+':
					result = Punctuator::PlusPlus;
					break;
				case '=':
					result = Punctuator::PlusEqual;
					break;
				default:
					return Punctuator::Plus;
			}
			break;
		case Punctuator::Percent:
			switch (*current) {
				case '>':
					result = Punctuator::RightBrace;
					break;
				case ':':
					return Punctuator::PartialHashHashDigraph;
				case '=':
					result = Punctuator::PercentEqual;
					break;
				default:
					return Punctuator::Percent;
			}
			break;
		case Punctuator::Equal:
			if (*current == '=')
				result = Punctuator::EqualEqual;
			else
				return Punctuator::Equal;
			break;
		case Punctuator::Ampersand:
			switch (*current) {
				case '&':
					result = Punctuator::AmpersandAmpersand;
					break;
				case '=':
					result = Punctuator::AmpersandEqual;
					break;
				default:
					return Punctuator::Ampersand;
			}
			break;
		case Punctuator::VerticalBar:
			switch (*current) {
				case '|':
					result = Punctuator::VerticalBarVerticalBar;
					break;
				case '=':
					result = Punctuator::VerticalBarEqual;
					break;
				default:
					return Punctuator::VerticalBar;
			}
			break;
		case Punctuator::LessThan:
			switch (*current) {
				case '<':
					return Punctuator::PartialLeftShift;
				case '=':
					result = Punctuator::LessThanEqual;
					break;
				case ':':
					result = Punctuator::LeftBracket;
					break;
				case '%':
					result = Punctuator::LeftBrace;
					break;
				default:
					return Punctuator::LessThan;
			}
			break;
		case Punctuator::GreaterThan:
			switch (*current) {
				case '>':
					return Punctuator::PartialRightShift;
				case '=':
					result = Punctuator::GreaterThanEqual;
					break;
				case ':':
					result = Punctuator::RightBracket;
					break;
				default:
					return Punctuator::GreaterThan;
			}
			break;
		case Punctuator::PartialLeftShift:
			if (*current == '=')
				result = Punctuator::LessThanLessThanEqual;
			else
				return Punctuator::LessThanLessThan;
			break;
		case Punctuator::PartialRightShift:
			if (*current == '=')
				result = Punctuator::GreaterThanGreaterThanEqual;
			else
				return Punctuator::GreaterThanGreaterThan;
			break;
		case Punctuator::PartialHashHashDigraph:
			if (*current == '%') {
				return Punctuator::PartialHashHashDigraphStage2;
			}

			return Punctuator::Hash;
		case Punctuator::PartialHashHashDigraphStage2:
			if (*current == ':') {
				result = Punctuator::HashHash;
			} else {
				return Punctuator::HashHashDigraphFailure;
			}
			break;
		case Punctuator::Hash:
			if (*current == '#') {
				result = Punctuator::HashHash;
			} else {
				return Punctuator::Hash;
			}
			break;
		case Punctuator::ExclamationMark:
			if (*current == '=') {
				result = Punctuator::ExclamationMarkEqual;
			} else {
				return Punctuator::ExclamationMark;
			}
			break;
		case Punctuator::Dot:
			if (*current == '.') {
				return Punctuator::PartialEllipsis;
			}
			return Punctuator::Dot;
		case Punctuator::PartialEllipsis:
			if (*current == '.') {
				result = Punctuator::Ellipsis;
			} else {
				return Punctuator::EllipsisFailure;
			}
			break;
		case Punctuator::Asterisk:
			if (*current == '=') {
				result = Punctuator::AsteriskEqual;
			} else {
				return Punctuator::Asterisk;
			}
			break;
		case Punctuator::Slash:
			if (*current == '=') {
				result = Punctuator::SlashEqual;
			} else {
				return Punctuator::Slash;
			}
			break;
		case Punctuator::Caret:
			if (*current == '=') {
				result = Punctuator::CaretEqual;
			} else {
				return Punctuator::Caret;
			}
			break;
		case Punctuator::Colon:
			if (*current == '>') {
				result = Punctuator::RightBracket;
			} else {
				return Punctuator::Colon;
			}
			break;
		default:
			// the previousPunctuator is itself a complete punctuator
			return prev;
	}

	++current;
	return result;
}

Preprocessor::TokenList Preprocessor::Tokenize(Diagnosis::Vec &diagnoses) {
	TokenList      tokens{};
	Punctuator     lastPunctuator{Punctuator::None};
	bool           wasLastPunctuatorHash{false};
	Token         *lastHash{nullptr};
	ConstantPrefix genericConstantPrefix{ConstantPrefix::None};

	for (StringConstIter current{m_Buffer.cbegin()}; current != m_Buffer.cend(); ++current) {
		if (*current != '\n' && isspace(*current)) {
			continue;
		}

		if (lastPunctuator != Punctuator::None) {
			const Punctuator punctuator{HandlePreviousPunctuator(current, lastPunctuator)};

			if (punctuator < Punctuator::None) {
				Token &inserted{tokens.emplace_back(punctuator)};

				if (punctuator == Punctuator::Hash) {
					wasLastPunctuatorHash = true;
					lastHash              = &inserted;
				}
			}

			if (current == m_Buffer.cend()) {
				if (punctuator > Punctuator::None) {
					const auto [one, two] = CollapsePartialPunctuator(punctuator);
					if (one != Punctuator::None) {
						tokens.emplace_back(one);
					}
					lastPunctuator = two;
				} else
					lastPunctuator = Punctuator::None;
				break;
			}

			if (punctuator > Punctuator::None) {
				lastPunctuator = punctuator;
				continue;
			}
		}

		if (*current == '\n') {
			// TODO: Handle line continuations
			// TODO: invalid in string literals
			lastPunctuator        = Punctuator::None;
			wasLastPunctuatorHash = false;
			continue;
		}

		// Punctuators
		switch (*current) {
			case '[':
				lastPunctuator = Punctuator::LeftBracket;
				continue;
			case ']':
				lastPunctuator = Punctuator::RightBracket;
				continue;
			case '{':
				lastPunctuator = Punctuator::LeftBrace;
				continue;
			case '}':
				lastPunctuator = Punctuator::RightBrace;
				continue;
			case '(':
				lastPunctuator = Punctuator::LeftParenthesis;
				continue;
			case ')':
				lastPunctuator = Punctuator::RightParenthesis;
				continue;
			case '.':
				lastPunctuator = Punctuator::Dot;
				continue;
			case '-':
				lastPunctuator = Punctuator::Minus;
				continue;
			case '+':
				lastPunctuator = Punctuator::Plus;
				continue;
			case '&':
				lastPunctuator = Punctuator::Ampersand;
				continue;
			case '|':
				lastPunctuator = Punctuator::VerticalBar;
				continue;
			case '*':
				lastPunctuator = Punctuator::Asterisk;
				continue;
			case '~':
				lastPunctuator = Punctuator::Tilde;
				continue;
			case '<':
				lastPunctuator = Punctuator::LessThan;
				continue;
			case '>':
				lastPunctuator = Punctuator::GreaterThan;
				continue;
			case '=':
				lastPunctuator = Punctuator::Equal;
				continue;
			case '!':
				lastPunctuator = Punctuator::ExclamationMark;
				continue;
			case '%':
				lastPunctuator = Punctuator::Percent;
				continue;
			case '?':
				lastPunctuator = Punctuator::QuestionMark;
				continue;
			case '^':
				lastPunctuator = Punctuator::Caret;
				continue;
			case '/':
				lastPunctuator = Punctuator::Slash;
				continue;
			case ',':
				lastPunctuator = Punctuator::Comma;
				continue;
			case ':':
				lastPunctuator = Punctuator::Colon;
				continue;
			case ';':
				lastPunctuator = Punctuator::Semicolon;
				continue;
			case '#':
				lastPunctuator = Punctuator::Hash;
				continue;
			default:
				lastPunctuator = Punctuator::None;
				break;
		}

		// TODO: Handle literals, as the keyword/identifier code relies on the character being a valid initial character
		if (genericConstantPrefix == ConstantPrefix::u && *current == '8') {
			genericConstantPrefix = ConstantPrefix::u8;
			continue;
		} else if (genericConstantPrefix == ConstantPrefix::None) {
			switch (*current) {
				case 'L':
					genericConstantPrefix = ConstantPrefix::L;
					continue;
				case 'u':
					genericConstantPrefix = ConstantPrefix::u;
					continue;
				case 'U':
					genericConstantPrefix = ConstantPrefix::U;
					continue;
				default:
					break;
			}
		}

		if (*current == '\'') {
			if (!TokenizeCharacterOrStringLiteral(
			            current, genericConstantPrefix, tokens, diagnoses, ConstantType::Character
			    ))
				return tokens;
			continue;
		}

		if (*current == '"') {
			if (!TokenizeCharacterOrStringLiteral(
			            current, genericConstantPrefix, tokens, diagnoses, ConstantType::String
			    ))
				return tokens;
			continue;
		}

		StringConstIter firstCharacter{current};

		if (genericConstantPrefix != ConstantPrefix::None) {
			if (genericConstantPrefix == ConstantPrefix::u8)
				firstCharacter -= 2;
			else
				--firstCharacter;
		}

		// never mind, the prefix is part of a keyword or identifier
		genericConstantPrefix = ConstantPrefix::None;

		// Keywords, Preprocessor Directives or Identifiers
		const StringConstIter end{std::find_if(firstCharacter, m_Buffer.cend(), [](auto c) {
			return !isalnum(c) && c != '_';
		})};
		if (end == firstCharacter)
			continue;

		const CompilerDataTypes::StringView content(firstCharacter, end);
		const CompilerDataTypes::String     contentStr{content};

		if (wasLastPunctuatorHash) {
			const std::optional<Directive> directive{MatchDirective(content)};
			if (directive.has_value()) {
				*lastHash = directive.value();
				current   = end - 1;// -1 because the for loop will increment it
				continue;
			}
		}

		const std::optional<Keyword> keyword{MatchKeyword(content)};

		if (keyword.has_value()) {
			tokens.emplace_back(keyword.value());
		} else {
			tokens.emplace_back(Identifier{contentStr});
		}
		current = end - 1;// -1 because the for loop will increment it
	}

	const auto [firstPunctuator, secondPunctuator] = CollapsePartialPunctuator(lastPunctuator);

	if (firstPunctuator != Punctuator::None) {
		tokens.emplace_back(firstPunctuator);
	}

	if (secondPunctuator != Punctuator::None) {
		tokens.emplace_back(secondPunctuator);
	}

	return tokens;
}

Preprocessor::TokenList Preprocessor::Process(Diagnosis::Vec &diagnoses) {
	if (m_Buffer.empty())
		return TokenList{};

	m_Buffer = std::regex_replace(m_Buffer, std::regex{"\r\n"}, "\n");

	TokenList tokenList{Tokenize(diagnoses)};

	return tokenList;
}

std::optional<Preprocessor::Keyword> Preprocessor::MatchKeyword(const CompilerDataTypes::StringView &view) const {
	const auto matchIt{m_Keywords.find(view)};

	if (matchIt == m_Keywords.cend())
		return std::nullopt;

	return matchIt->second;
}

std::optional<Preprocessor::Directive> Preprocessor::MatchDirective(const CompilerDataTypes::StringView &view) const {
	const auto matchIt{m_Directives.find(view)};

	if (matchIt == m_Directives.cend())
		return std::nullopt;

	return matchIt->second;
}

bool Preprocessor::TokenizeNumericalEscapeSequence(
        StringConstIter &current, CompilerDataTypes::String &literalContent, Diagnosis::Vec &diagnoses,
        unsigned long valueLimit, uint32_t valueMask, ValidEscapeBase base
) {
	StringConstIter endOfHex{};

	if (base == ValidEscapeBase::Octal) {
		size_t charsRead{0};
		endOfHex = current + 1;
		for (StringConstIter it{current}; it != m_Buffer.cend() && charsRead != NUM_DIGITS_OCTAL_ESCAPE;
		     ++it, ++charsRead) {
			if (*it < '0' || *it > '7') {
				break;
			}
			endOfHex = it + 1;
		}
	} else /* hex */ {
		++current;// skip the 'x'
		if (current == m_Buffer.cend()) {
			const Span span{};// TODO: Get the span
			diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_UnexpectedEOF, *current);
			return false;
		}

		endOfHex = std::find_if(current, m_Buffer.cend(), [=](auto c) { return !isxdigit(c); });

		if (endOfHex == current) {
			const Span span{};// TODO: Get the span
			diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_CharHexNoDigits, *current);
			return false;
		}

		// endOfHex may be m_Buffer.cend() here but that's fine because that will be checked later
	}

	const String  chars{current, endOfHex};
	unsigned long value{std::stoul(chars, nullptr, base == ValidEscapeBase::Octal ? 8 : 16)};
	if (value > valueLimit) {
		const Span span{};// TODO: Get the span
		diagnoses.emplace_back(span, Diagnosis::Class::Warning, Diagnosis::Kind::PP_CharOutOfRange);
		value &= valueMask;
	}

	literalContent += static_cast<CompilerDataTypes::Char>(value);
	current = endOfHex - 1;
	return true;
}

bool Preprocessor::TokenizeCharacterOrStringLiteral(
        CompilerDataTypes::String::const_iterator &current, ConstantPrefix prefix, TokenList &tokens,
        Diagnosis::Vec &diagnoses, Preprocessor::ConstantType type
) {
	bool                      isEscaped{false};
	CompilerDataTypes::String literalContent{};
	++current;

	int      charValueLimit{};
	uint32_t charValueMask{};
	uint32_t charConstValueMask{};

	switch (prefix) {
		case ConstantPrefix::None:
			charValueLimit     = CompilerDataTypeInfo::CHAR::max();
			charValueMask      = CompilerDataTypeInfo::CHAR::MASK;
			charConstValueMask = CompilerDataTypeInfo::INT::MASK;
			break;
		case ConstantPrefix::L:
			charValueLimit     = CompilerDataTypeInfo::WCHAR_T::max();
			charValueMask      = CompilerDataTypeInfo::WCHAR_T::MASK;
			charConstValueMask = CompilerDataTypeInfo::WCHAR_T::MASK;
			break;
		case ConstantPrefix::u:
			charValueLimit     = CompilerDataTypeInfo::CHAR16_T::max();
			charValueMask      = CompilerDataTypeInfo::CHAR16_T::MASK;
			charConstValueMask = CompilerDataTypeInfo::CHAR16_T::MASK;
			break;
		case ConstantPrefix::U:
			charValueLimit     = CompilerDataTypeInfo::CHAR32_T::max();
			charValueMask      = CompilerDataTypeInfo::CHAR32_T::MASK;
			charConstValueMask = CompilerDataTypeInfo::CHAR32_T::MASK;
			break;
		case ConstantPrefix::u8:
			charValueLimit = CompilerDataTypeInfo::CHAR::max();
			charValueMask  = CompilerDataTypeInfo::CHAR::MASK;
			// charConstValueMask is not relevant for character literals
			break;
		default:
			break;
	}

	for (; current != m_Buffer.cend(); ++current) {
		// TODO: move to functor

		const char c{*current};

		if (c == '\\' && !isEscaped) {
			isEscaped = true;
			continue;
		}

		if (isEscaped) {
			// TODO: move to helper function
			isEscaped = false;
			switch (c) {
				case 'n':
					literalContent += '\n';
					continue;
				case 't':
					literalContent += '\t';
					continue;
				case 'v':
					literalContent += '\v';
					continue;
				case 'b':
					literalContent += '\b';
					continue;
				case 'f':
					literalContent += '\f';
					continue;
				case 'r':
					literalContent += '\r';
					continue;
				case 'a':
					literalContent += '\a';
					continue;
				case '\\':
					literalContent += '\\';
					continue;
				case '?':
					literalContent += '?';
					continue;
				case '"':
					literalContent += '"';
					continue;
				case '\'':
					literalContent += '\'';
					continue;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					TokenizeNumericalEscapeSequence(
					        current, literalContent, diagnoses, charValueLimit, charValueMask, ValidEscapeBase::Octal
					);
					continue;
				case 'x': {
					if (!TokenizeNumericalEscapeSequence(
					            current, literalContent, diagnoses, charValueLimit, charValueMask,
					            ValidEscapeBase::Hexadecimal
					    ))
						return false;
					continue;
				}
				default:
					const Span span{};// TODO: Get the span
					diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_UnknownEscapeSequence, c);
					return false;
			}
		}

		if (c > charValueLimit) {
			const Span span{};// TODO: Get the span
			diagnoses.emplace_back(span, Diagnosis::Class::Warning, Diagnosis::Kind::PP_CharOutOfRange);
			literalContent += static_cast<CompilerDataTypes::Char>(c);
			continue;
		}

		const auto terminator{type == ConstantType::String ? '"' : '\''};
		const auto result{(c == terminator || c == '\n') && !isEscaped};

		isEscaped = false;

		if (result) {
			break;
		}

		literalContent += c;
	}

	if (type == ConstantType::String) {
		if (current == m_Buffer.cend() || *current != '"') {
			const Span span{};// TODO: Get the span
			diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_StrUnterminated);
			return false;
		}

		tokens.emplace_back(StringConstant{literalContent, prefix});
		return true;
	}
	// TODO: move to helper function

	if (current == m_Buffer.cend() || *current != '\'') {
		const Span span{};// TODO: Get the span
		diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_CharUnterminated);
		return false;
	}

	bool willBeTruncated;
	switch (prefix) {
		case ConstantPrefix::L:
		case ConstantPrefix::None:
			willBeTruncated = literalContent.size() > 1;
			break;
		case ConstantPrefix::u:
			willBeTruncated = literalContent.size() > 2;
			break;
		case ConstantPrefix::U:
			willBeTruncated = literalContent.size() > 4;
			break;
		case ConstantPrefix::u8:
			// not relevant for characters
			break;
	}

	if (willBeTruncated) {
		const Span span{};// TODO: Get the span
		diagnoses.emplace_back(span, Diagnosis::Class::Warning, Diagnosis::Kind::PP_CharOutOfRange);
	}

	// The behavior for character literals with multiple characters is implementation defined.
	// This implementation will add up to 4 characters to the value depending on the prefix.
	// This implementation will also truncate individual characters to the value limit.
	uint32_t value{};
	switch (literalContent.size()) {
		case 4:
			value |= (literalContent[3] & CompilerDataTypeInfo::CHAR::MASK);
			value |= (literalContent[2] & CompilerDataTypeInfo::CHAR::MASK) << 8u;
			value |= (literalContent[1] & CompilerDataTypeInfo::CHAR::MASK) << 16u;
			value |= (literalContent[0] & CompilerDataTypeInfo::CHAR::MASK) << 24u;

			break;
		case 3:
			value |= literalContent[2] & CompilerDataTypeInfo::CHAR::MASK;
			value |= (literalContent[1] & CompilerDataTypeInfo::CHAR::MASK) << 8u;
			value |= (literalContent[0] & CompilerDataTypeInfo::CHAR::MASK) << 16u;

			break;
		case 2:
			value |= literalContent[1] & CompilerDataTypeInfo::CHAR::MASK;
			value |= (literalContent[0] & CompilerDataTypeInfo::CHAR::MASK) << 8u;

			break;
		case 1:
			value |= literalContent[0] & CompilerDataTypeInfo::CHAR::MASK;

			break;
		default:
			const Span span{};// TODO: Get the span
			diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_CharNoValue);
			return false;
	}
	value &= charConstValueMask;

	tokens.emplace_back(CharacterConstant{value, prefix});
	return true;
}

std::ostream &operator<<(std::ostream &os, Preprocessor::Punctuator punctuator) {
	return os << magic_enum::enum_name(punctuator);
}

std::ostream &operator<<(std::ostream &os, Preprocessor::Keyword keyword) {
	return os << magic_enum::enum_name(keyword);
}

std::ostream &operator<<(std::ostream &os, Preprocessor::Directive directive) {
	return os << magic_enum::enum_name(directive);
}

void PrintTo(const Preprocessor::StringConstant &stringConstant, std::ostream *os) {
	if (stringConstant.m_Prefix != Preprocessor::ConstantPrefix::None)
		*os << '(' << magic_enum::enum_name(stringConstant.m_Prefix) << ") ";

	*os << testing::PrintToString(stringConstant.m_String);
}

void PrintTo(const Preprocessor::CharacterConstant &characterConstant, std::ostream *os) {
	if (characterConstant.m_Prefix != Preprocessor::ConstantPrefix::None)
		*os << '(' << magic_enum::enum_name(characterConstant.m_Prefix) << ") ";

	if (characterConstant.m_Character <= CompilerDataTypeInfo::CHAR::max() && characterConstant.m_Character > ' ') {
		*os << '\'' << static_cast<char>(characterConstant.m_Character) << '\'';
		return;
	}

	*os << "U+" << std::hex << characterConstant.m_Character;
}
