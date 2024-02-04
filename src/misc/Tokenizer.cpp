#include "Tokenizer.h"
#include "Diagnosis.h"
#include "compiler_data_types.h"
#include <magic_enum/magic_enum.hpp>
#include <regex>

Tokenizer::Punctuator Tokenizer::TokenizeDot() {
	if (!m_Current || *m_Current != '.') {
		return Punctuator::Dot;
	}

	m_Current.Next();
	if (!m_Current || m_Current++ != '.') {
		const Span span{};// TODO: Get the span
		m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_PartialTokenEncountered, "...");
		return Punctuator::None;
	}

	return Punctuator::Ellipsis;
}

Tokenizer::Punctuator Tokenizer::TokenizeDash() {
	if (!m_Current) {
		return Punctuator::Minus;
	}

	Punctuator result;

	switch (*m_Current) {
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

	++m_Current;
	return result;
}

Tokenizer::Punctuator Tokenizer::TokenizePlus() {
	if (!m_Current) {
		return Punctuator::Plus;
	}

	Punctuator result;

	switch (*m_Current) {
		case '+':
			result = Punctuator::PlusPlus;
			break;
		case '=':
			result = Punctuator::PlusEqual;
			break;
		default:
			return Punctuator::Plus;
	}

	++m_Current;
	return result;
}

Tokenizer::Punctuator Tokenizer::TokenizeAmpersand() {
	if (!m_Current) {
		return Punctuator::Ampersand;
	}

	Punctuator result;

	switch (*m_Current) {
		case '&':
			result = Punctuator::AmpersandAmpersand;
			break;
		case '=':
			result = Punctuator::AmpersandEqual;
			break;
		default:
			return Punctuator::Ampersand;
	}

	++m_Current;
	return result;
}

Tokenizer::Punctuator Tokenizer::TokenizeVerticalBar() {
	if (!m_Current) {
		return Punctuator::VerticalBar;
	}

	Punctuator result;

	switch (*m_Current) {
		case '|':
			result = Punctuator::VerticalBarVerticalBar;
			break;
		case '=':
			result = Punctuator::VerticalBarEqual;
			break;
		default:
			return Punctuator::VerticalBar;
	}

	++m_Current;
	return result;
}

Tokenizer::Punctuator Tokenizer::TokenizeAsterisk() {
	if (!m_Current || *m_Current != '=') {
		return Punctuator::Asterisk;
	}

	++m_Current;
	return Punctuator::AsteriskEqual;
}

Tokenizer::Punctuator Tokenizer::TokenizeLessThan() {
	if (!m_Current) {
		return Punctuator::LessThan;
	}

	Punctuator result;

	switch (*m_Current) {
		case '<':
			++m_Current;
			return TokenizeDoubleLessThan();
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

	++m_Current;
	return result;
}

Tokenizer::Punctuator Tokenizer::TokenizeDoubleLessThan() {
	if (!m_Current || *m_Current != '=') {
		return Punctuator::LessThanLessThan;
	}

	++m_Current;
	return Punctuator::LessThanLessThanEqual;
}

Tokenizer::Punctuator Tokenizer::TokenizeGreaterThan() {
	if (!m_Current) {
		return Punctuator::GreaterThan;
	}

	Punctuator result;

	switch (*m_Current) {
		case '>':
			++m_Current;
			return TokenizeDoubleGreaterThan();
		case '=':
			result = Punctuator::GreaterThanEqual;
			break;
		case ':':
			result = Punctuator::RightBracket;
			break;
		default:
			return Punctuator::GreaterThan;
	}

	++m_Current;
	return result;
}

Tokenizer::Punctuator Tokenizer::TokenizeDoubleGreaterThan() {
	if (!m_Current || *m_Current != '=') {
		return Punctuator::GreaterThanGreaterThan;
	}

	++m_Current;
	return Punctuator::GreaterThanGreaterThanEqual;
}

Tokenizer::Punctuator Tokenizer::TokenizeEqual() {
	if (!m_Current || *m_Current != '=') {
		return Punctuator::Equal;
	}

	++m_Current;
	return Punctuator::EqualEqual;
}

Tokenizer::Punctuator Tokenizer::TokenizeExclamationMark() {
	if (!m_Current || *m_Current != '=') {
		return Punctuator::ExclamationMark;
	}

	++m_Current;
	return Punctuator::ExclamationMarkEqual;
}

Tokenizer::Token Tokenizer::TokenizePercent() {
	if (!m_Current) {
		return Punctuator::Percent;
	}

	Punctuator result;

	switch (*m_Current) {
		case '>':
			result = Punctuator::RightBrace;
			break;
		case ':':
			++m_Current;
			return TokenizeHashHashDigraph();
		case '=':
			result = Punctuator::PercentEqual;
			break;
		default:
			return Punctuator::Percent;
	}

	++m_Current;
	return result;
}

Tokenizer::Token Tokenizer::TokenizeHashHashDigraph() {
	if (!m_Current || *m_Current != '%') {
		return Punctuator::Hash;
	}

	m_Current.Next();
	if (!m_Current || m_Current++ != ':') {
		const Span span{};// TODO: Get the span
		m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_PartialTokenEncountered, "%:%:");
		return SpecialPurpose::Error;
	}

	return Punctuator::HashHash;
}

Tokenizer::Punctuator Tokenizer::TokenizeCaret() {
	if (!m_Current || *m_Current != '=') {
		return Punctuator::Caret;
	}

	++m_Current;
	return Punctuator::CaretEqual;
}

Tokenizer::Punctuator Tokenizer::TokenizeSlash() {
	if (!m_Current) {
		return Punctuator::Slash;
	}

	switch (*m_Current) {
		case '/':
			while (*m_Current != '\n') ++m_Current;
			return Punctuator::None;
		case '=':
			++m_Current;
			return Punctuator::SlashEqual;
		default:
			return Punctuator::Slash;
	}
}

Tokenizer::Punctuator Tokenizer::TokenizeColon() {
	if (!m_Current || *m_Current != '>') {
		return Punctuator::Colon;
	}

	++m_Current;
	return Punctuator::RightBracket;
}

Tokenizer::Punctuator Tokenizer::TokenizeHash() {
	if (!m_Current || *m_Current != '#') {
		return Punctuator::Hash;
	}

	++m_Current;
	return Punctuator::HashHash;
}

Tokenizer::Token Tokenizer::TokenizePunctuator() {
	Punctuator result;

	switch (*m_Current) {
		case '[':
			result = Punctuator::LeftBracket;
			break;
		case ']':
			result = Punctuator::RightBracket;
			break;
		case '{':
			result = Punctuator::LeftBrace;
			break;
		case '}':
			result = Punctuator::RightBrace;
			break;
		case '(':
			result = Punctuator::LeftParenthesis;
			break;
		case ')':
			result = Punctuator::RightParenthesis;
			break;
		case ';':
			result = Punctuator::Semicolon;
			break;
		case '~':
			result = Punctuator::Tilde;
			break;
		case '?':
			// trigraphs are replaced with their corresponding character before tokenization
			result = Punctuator::QuestionMark;
			break;
		case ',':
			result = Punctuator::Comma;
			break;
		case '-':
			++m_Current;
			return TokenizeDash();
		case '+':
			++m_Current;
			return TokenizePlus();
		case '&':
			++m_Current;
			return TokenizeAmpersand();
		case '|':
			++m_Current;
			return TokenizeVerticalBar();
		case '*':
			++m_Current;
			return TokenizeAsterisk();
		case '<':
			++m_Current;
			return TokenizeLessThan();
		case '>':
			++m_Current;
			return TokenizeGreaterThan();
		case '=':
			++m_Current;
			return TokenizeEqual();
		case '!':
			++m_Current;
			return TokenizeExclamationMark();
		case '^':
			++m_Current;
			return TokenizeCaret();
		case '/':
			++m_Current;
			return TokenizeSlash();
		case ':':
			++m_Current;
			return TokenizeColon();
		case '#':
			++m_Current;
			return TokenizeHash();
		case '.':
			++m_Current;
			return TokenizeDot();
		case '%':
			++m_Current;
			return TokenizePercent();
		default:
			return Punctuator::None;
	}

	++m_Current;
	return result;
}

std::optional<Tokenizer::Token> Tokenizer::TokenizeDirective() {
	if (!m_Current)
		return Punctuator::Hash;

	if (*m_Current == '#') {
		m_Current.Next();
		return Punctuator::HashHash;
	}

	while (m_Current.Good() && isspace(*m_Current) && m_Current != '\n') m_Current.Next();

	if (!m_Current || *m_Current == '\n')
		return Punctuator::Hash;

	const auto *trieNode{&m_DirectiveTrie};

	while (m_Current.Good() && trieNode != nullptr) {
		trieNode = trieNode->GetNode(m_Current);
		m_Current.Next();
	}

	if (trieNode != nullptr && trieNode->m_Leaf.has_value())
		return trieNode->m_Leaf->m_Value;

	return std::nullopt;
}

Tokenizer::Token Tokenizer::TokenizeIdentifierOrKeyword() {
	std::basic_string<char32_t> identifierContents{};
	ConstantPrefix              prefix{ConstantPrefix::None};
	const KeywordTrie          *trieNode{nullptr};

	if (!m_Current.GetIsEscapeChar()) {
		switch (*m_Current) {
			case 'u':
				trieNode = m_KeywordTrie.GetNode(m_Current);
				identifierContents += m_Current++;

				prefix = ConstantPrefix::u;
				if (m_Current == '8') {
					// No keyword starts with u8, so it must either be a character literal or an identifier.
					// No use checking the trie.

					identifierContents += m_Current++;

					if (!m_Current)
						return Identifier{U"u8"};

					prefix = ConstantPrefix::u8;
				}
				break;
			case 'U':
				trieNode = m_KeywordTrie.GetNode(m_Current);
				identifierContents += m_Current++;
				prefix = ConstantPrefix::U;
				break;
			case 'L':
				trieNode = m_KeywordTrie.GetNode(m_Current);
				identifierContents += m_Current++;
				prefix = ConstantPrefix::L;
				break;
		}

		switch (*m_Current) {
			case '\'':
				m_Current.SimpleNext();
				return TokenizeCharacterOrStringLiteral(prefix, ConstantType::Character);
			case '"':
				m_Current.SimpleNext();
				return TokenizeCharacterOrStringLiteral(prefix, ConstantType::String);
		}
	}

	if (trieNode == nullptr)
		trieNode = &m_KeywordTrie;

	while (m_Current.Good() && (isalnum(*m_Current) || *m_Current == '_' || *m_Current == '\\')) {
		if (m_Current.GetIsEscapeChar()) {
			if (!m_Current)
				return SpecialPurpose::EndOfFile;

			UniversalCharacterNameType type;

			switch (*m_Current) {
				case 'u':
					type = UniversalCharacterNameType::u;
					break;
				case 'U':
					type = UniversalCharacterNameType::U;
					break;
				default:
					const Span span{};// TODO: Get the span
					m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_IllegalBackslash);
					return SpecialPurpose::Error;
			}

			m_Current.Next();
			if (const auto universalChar{TokenizeUniversalCharacterName(type)}; universalChar.has_value()) {
				identifierContents += universalChar.value();
			}
		} else {
			identifierContents += *m_Current;
		}

		if (trieNode != nullptr && m_Current.Good())
			trieNode = trieNode->GetNode(m_Current);

		m_Current.Next();
	}

	if (trieNode != nullptr && trieNode->m_Leaf.has_value()) {
		return trieNode->m_Leaf->m_Value;
	}

	return Identifier{identifierContents};
}

Tokenizer::Token Tokenizer::Tokenize() {
	// skip whitespace
	while (m_Current.Good() && isspace(*m_Current)) ++m_Current;

	if (!m_Current)
		return SpecialPurpose::EndOfFile;

	if (*m_Current == '#') {
		m_Current.Next();

		// directives must start on a new line
		const auto potentialDirective{TokenizeDirective()};
		if (potentialDirective.has_value()) {
			return potentialDirective.value();
		}

		return TokenizeHash();
	}

	// Punctuators
	const auto punctuatorOrError{TokenizePunctuator()};
	if (std::holds_alternative<Punctuator>(punctuatorOrError)) {
		if (const Punctuator punctuator{std::get<Punctuator>(punctuatorOrError)}; punctuator != Punctuator::None) {
			return punctuator;
		}
	} else if (std::holds_alternative<SpecialPurpose>(punctuatorOrError)) {
		return SpecialPurpose::Error;
	}

	return TokenizeIdentifierOrKeyword();
}

std::optional<CompilerDataTypes::Char> Tokenizer::TokenizeNumericalEscapeSequence(ValidEscapeBase base) {
	if (base == ValidEscapeBase::Octal) {
		// *m_Current is the first digit of the escape sequence
		int value{};
		for (size_t digit{0}; digit < NUM_DIGITS_OCTAL_ESCAPE && m_Current.Good(); ++digit) {
			if (*m_Current < '0' || *m_Current > '7') {
				break;
			}

			value <<= 3u;
			value |= *m_Current - '0';
			m_Current.Next();
		}
		value &= 0xFF;
		return static_cast<char>(value);
	}

	// hex, first character is 'x'
	m_Current.Next();

	if (!m_Current) {
		const Span span{};// TODO: Get the span
		m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_UnexpectedEOF, *m_Current);
		return std::nullopt;
	}

	if (!isxdigit(*m_Current)) {
		const Span span{};// TODO: Get the span
		m_Diagnoses.emplace_back(
		        span, Diagnosis::Class::Error, Diagnosis::Kind::TK_CharHexNoDigits, static_cast<char>(*m_Current)
		);
		return std::nullopt;
	}

	int value{};
	while (m_Current.Good()) {
		if (const auto optionalDigitValue{TokenizeHexDigit()}; optionalDigitValue.has_value()) {
			value <<= 4u;
			value |= optionalDigitValue.value();
		} else {
			break;
		}
	}

	return static_cast<char>(value);
}

std::optional<char> Tokenizer::TokenizeEscapeSequence() {
	switch (*m_Current) {
		case 'n':
			m_Current.Next();
			return '\n';
		case 't':
			m_Current.Next();
			return '\t';
		case 'v':
			m_Current.Next();
			return '\v';
		case 'b':
			m_Current.Next();
			return '\b';
		case 'f':
			m_Current.Next();
			return '\f';
		case 'r':
			m_Current.Next();
			return '\r';
		case 'a':
			m_Current.Next();
			return '\a';
		case '\\':
			m_Current.Next();
			return '\\';
		case '?':
			m_Current.Next();
			return '?';
		case '"':
			m_Current.Next();
			return '"';
		case '\'':
			m_Current.Next();
			return '\'';
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			return TokenizeNumericalEscapeSequence(ValidEscapeBase::Octal);
		case 'x':
			return TokenizeNumericalEscapeSequence(ValidEscapeBase::Hexadecimal);
		case 'u':
			m_Current.Next();
			return TokenizeUniversalCharacterName(UniversalCharacterNameType::u);
		case 'U':
			m_Current.Next();
			return TokenizeUniversalCharacterName(UniversalCharacterNameType::U);
		default:
			const Span span{};// TODO: Get the span
			m_Diagnoses.emplace_back(
			        span, Diagnosis::Class::Error, Diagnosis::Kind::TK_UnknownEscapeSequence,
			        static_cast<char>(*m_Current)
			);
			return std::nullopt;
	}
}

[[nodiscard]]
bool IsLegalUniversalCharacterName(char32_t value) {
	constexpr char32_t minCodePoint{0x00A0};
	constexpr char32_t illegalRangeStartIncl{0xD800}, illegalRangeEndIncl{0xDFFF};

	if (value < minCodePoint && value != U'$' && value != U'@' && value != U'`')
		return false;

	return value < illegalRangeStartIncl || value > illegalRangeEndIncl;
}

std::optional<char32_t> Tokenizer::TokenizeUniversalCharacterName(UniversalCharacterNameType type) {
	// *m_Current is the first digit of the escape sequence
	char32_t     value{};
	const size_t numDigits{type == UniversalCharacterNameType::u ? 4ull : 8ull};
	size_t       digitIdx{0};

	for (; digitIdx < numDigits && m_Current.Good(); ++digitIdx) {
		if (const auto optionalDigitValue{TokenizeHexDigit()}; optionalDigitValue.has_value()) {
			value <<= 4u;
			value |= optionalDigitValue.value();
			continue;
		}

		break;
	}

	if (digitIdx != numDigits) {
		const Span span{};// TODO: Get the span
		m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_InvalidUniversalCharacterName);
		return std::nullopt;
	}

	if (!IsLegalUniversalCharacterName(value)) {
		const Span span{};//TODO: Get the span
		m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_IllegalUniversalCharacterName);
		return std::nullopt;
	}

	return value;
}

Tokenizer::Token Tokenizer::TokenizeCharacterOrStringLiteral(ConstantPrefix prefix, Tokenizer::ConstantType type) {
	if (!m_Current)
		return SpecialPurpose::Error;

	CompilerDataTypes::String literalContent{};
	const auto                terminator{type == ConstantType::String ? '"' : '\''};

	uint32_t charConstValueMask{};

	switch (prefix) {
		case ConstantPrefix::None:
			charConstValueMask = CompilerDataTypeInfo::INT::MASK;
			break;
		case ConstantPrefix::L:
			charConstValueMask = CompilerDataTypeInfo::WCHAR_T::MASK;
			break;
		case ConstantPrefix::u:
			charConstValueMask = CompilerDataTypeInfo::CHAR16_T::MASK;
			break;
		case ConstantPrefix::U:
			charConstValueMask = CompilerDataTypeInfo::CHAR32_T::MASK;
			break;
		case ConstantPrefix::u8:
			// charConstValueMask is not relevant for character literals
		default:
			break;
	}

	while (m_Current.Good()) {
		// TODO: move to functor

		if (*m_Current == '\\') {
			m_Current.SimpleNext();

			if (!m_Current) {
				const Span span{};// TODO: Get the span
				m_Diagnoses.emplace_back(
				        span, Diagnosis::Class::Error, Diagnosis::Kind::TK_UnexpectedEOF, static_cast<char>(*m_Current)
				);
				return SpecialPurpose::EndOfFile;
			}

			if (m_Current == '\n') {
				m_Current.SimpleNext();
				continue;
			}

			if (const auto escape{TokenizeEscapeSequence()}; escape.has_value()) {
				literalContent += escape.value();
				continue;
			}

			return SpecialPurpose::Error;
		}

		const auto result{*m_Current == terminator};

		if (result) {
			break;
		}

		literalContent += static_cast<char>(*m_Current);
		m_Current.SimpleNext();
	}

	if (type == ConstantType::String) {
		if (!m_Current || *m_Current != '"') {
			const Span span{};// TODO: Get the span
			m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_StrUnterminated);
			return SpecialPurpose::Error;
		}

		return StringConstant{literalContent, prefix};
	}
	// TODO: move to helper function

	if (m_Current != '\'') {
		const Span span{};// TODO: Get the span
		m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_CharUnterminated);
		return SpecialPurpose::Error;
	}

	bool willBeTruncated{false};
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
		m_Diagnoses.emplace_back(span, Diagnosis::Class::Warning, Diagnosis::Kind::TK_CharOutOfRange);
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
			m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_CharNoValue);
			return SpecialPurpose::Error;
	}
	value &= charConstValueMask;

	return CharacterConstant{value, prefix};
}

Tokenizer::Token Tokenizer::operator()() {
	if (!m_Current)
		return SpecialPurpose::EndOfFile;

	m_Current.Next();

	try {
		return Tokenize();
	} catch (InvalidBackslashException &) {
		const Span span{};
		m_Diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::TK_IllegalBackslash);
		return SpecialPurpose::Error;
	}
}

std::ostream &operator<<(std::ostream &os, Tokenizer::SpecialPurpose specialPurpose) {
	return os << magic_enum::enum_name(specialPurpose);
}

std::ostream &operator<<(std::ostream &os, Tokenizer::Punctuator punctuator) {
	return os << magic_enum::enum_name(punctuator);
}

std::ostream &operator<<(std::ostream &os, Tokenizer::Keyword keyword) {
	return os << magic_enum::enum_name(keyword);
}

std::ostream &operator<<(std::ostream &os, Tokenizer::Directive directive) {
	return os << magic_enum::enum_name(directive);
}

void PrintTo(const Tokenizer::StringConstant &stringConstant, std::ostream *os) {
	if (stringConstant.m_Prefix != Tokenizer::ConstantPrefix::None)
		*os << '(' << magic_enum::enum_name(stringConstant.m_Prefix) << ") ";

	*os << testing::PrintToString(stringConstant.m_String);
}

void PrintTo(const Tokenizer::CharacterConstant &characterConstant, std::ostream *os) {
	if (characterConstant.m_Prefix != Tokenizer::ConstantPrefix::None)
		*os << '(' << magic_enum::enum_name(characterConstant.m_Prefix) << ") ";

	if (characterConstant.m_Character <= CompilerDataTypeInfo::CHAR::max() && characterConstant.m_Character > ' ') {
		*os << '\'' << static_cast<char>(characterConstant.m_Character) << '\'';
		return;
	}

	*os << "U+" << std::hex << characterConstant.m_Character;
}

std::optional<int> Tokenizer::TokenizeHexDigit() {
	if (!isxdigit(*m_Current)) {
		return std::nullopt;
	}

	int value;

	if (isdigit(*m_Current))
		value = *m_Current - '0';
	else if (isupper(*m_Current))
		value = *m_Current - 'A' + 10;
	else
		value = *m_Current - 'a' + 10;

	m_Current.Next();
	return value;
}
