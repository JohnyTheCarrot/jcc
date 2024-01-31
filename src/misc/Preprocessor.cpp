#include "Preprocessor.h"
#include "Diagnosis.h"
#include "compiler_data_types.h"
#include <magic_enum/magic_enum.hpp>
#include <regex>

Preprocessor::Punctuator Preprocessor::TokenizeDot(Diagnosis::Vec &diagnoses) {
	if (m_Current == m_Buffer.cend() || *m_Current != '.') {
		return Punctuator::Dot;
	}

	if (++m_Current == m_Buffer.cend() || *m_Current++ != '.') {
		const Span span{};// TODO: Get the span
		diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_PartialTokenEncountered, "...");
		return Punctuator::None;
	}

	return Punctuator::Ellipsis;
}

Preprocessor::Punctuator Preprocessor::TokenizeDash() {
	if (m_Current == m_Buffer.cend()) {
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

Preprocessor::Punctuator Preprocessor::TokenizePlus() {
	if (m_Current == m_Buffer.cend()) {
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

Preprocessor::Punctuator Preprocessor::TokenizeAmpersand() {
	if (m_Current == m_Buffer.cend()) {
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

Preprocessor::Punctuator Preprocessor::TokenizeVerticalBar() {
	if (m_Current == m_Buffer.cend()) {
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

Preprocessor::Punctuator Preprocessor::TokenizeAsterisk() {
	if (m_Current == m_Buffer.cend() || *m_Current != '=') {
		return Punctuator::Asterisk;
	}

	++m_Current;
	return Punctuator::AsteriskEqual;
}

Preprocessor::Punctuator Preprocessor::TokenizeLessThan() {
	if (m_Current == m_Buffer.cend()) {
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

Preprocessor::Punctuator Preprocessor::TokenizeDoubleLessThan() {
	if (m_Current == m_Buffer.cend() || *m_Current != '=') {
		return Punctuator::LessThanLessThan;
	}

	++m_Current;
	return Punctuator::LessThanLessThanEqual;
}

Preprocessor::Punctuator Preprocessor::TokenizeGreaterThan() {
	if (m_Current == m_Buffer.cend()) {
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

Preprocessor::Punctuator Preprocessor::TokenizeDoubleGreaterThan() {
	if (m_Current == m_Buffer.cend() || *m_Current != '=') {
		return Punctuator::GreaterThanGreaterThan;
	}

	++m_Current;
	return Punctuator::GreaterThanGreaterThanEqual;
}

Preprocessor::Punctuator Preprocessor::TokenizeEqual() {
	if (m_Current == m_Buffer.cend() || *m_Current != '=') {
		return Punctuator::Equal;
	}

	++m_Current;
	return Punctuator::EqualEqual;
}

Preprocessor::Punctuator Preprocessor::TokenizeExclamationMark() {
	if (m_Current == m_Buffer.cend() || *m_Current != '=') {
		return Punctuator::ExclamationMark;
	}

	++m_Current;
	return Punctuator::ExclamationMarkEqual;
}

Preprocessor::Punctuator Preprocessor::TokenizePercent(Diagnosis::Vec &diagnoses) {
	if (m_Current == m_Buffer.cend()) {
		return Punctuator::Percent;
	}

	Punctuator result;

	switch (*m_Current) {
		case '>':
			result = Punctuator::RightBrace;
			break;
		case ':':
			++m_Current;
			return TokenizeHashHashDigraph(diagnoses);
		case '=':
			result = Punctuator::PercentEqual;
			break;
		default:
			return Punctuator::Percent;
	}

	++m_Current;
	return result;
}

Preprocessor::Punctuator Preprocessor::TokenizeHashHashDigraph(Diagnosis::Vec &diagnoses) {
	if (m_Current == m_Buffer.cend() || *m_Current != '%') {
		return Punctuator::Hash;
	}

	if (++m_Current == m_Buffer.cend() || *m_Current++ != ':') {
		const Span span{};// TODO: Get the span
		diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_PartialTokenEncountered, "%:%:");
		return Punctuator::None;
	}

	return Punctuator::HashHash;
}

Preprocessor::Punctuator Preprocessor::TokenizeCaret() {
	if (m_Current == m_Buffer.cend() || *m_Current != '=') {
		return Punctuator::Caret;
	}

	++m_Current;
	return Punctuator::CaretEqual;
}

Preprocessor::Punctuator Preprocessor::TokenizeSlash() {
	if (m_Current == m_Buffer.cend()) {
		return Punctuator::Slash;
	}

	switch (*m_Current) {
		case '/':
			m_Current = std::find(m_Current, m_Buffer.cend(), '\n');
			return Punctuator::None;
		case '=':
			++m_Current;
			return Punctuator::SlashEqual;
		default:
			return Punctuator::Slash;
	}
}

Preprocessor::Punctuator Preprocessor::TokenizeColon() {
	if (m_Current == m_Buffer.cend() || *m_Current != '>') {
		return Punctuator::Colon;
	}

	++m_Current;
	return Punctuator::RightBracket;
}

Preprocessor::Punctuator Preprocessor::TokenizeHash() {
	if (m_Current == m_Buffer.cend() || *m_Current != '#') {
		return Punctuator::Hash;
	}

	++m_Current;
	return Punctuator::HashHash;
}

Preprocessor::Punctuator Preprocessor::TokenizePunctuator(Diagnosis::Vec &diagnoses) {
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
			return TokenizeDot(diagnoses);
		case '%':
			++m_Current;
			return TokenizePercent(diagnoses);
		default:
			return Punctuator::None;
	}

	++m_Current;
	return result;
}

std::optional<Preprocessor::Directive> Preprocessor::TokenizeDirective(Diagnosis::Vec &diagnoses) {
	m_Current = std::find_if(m_Current, m_Buffer.cend(), [](auto c) { return !isspace(c) || c == '\n'; });

	if (m_Current == m_Buffer.cend())
		return std::nullopt;

	if (*m_Current == '\n') {
		++m_Current;
		return std::nullopt;
	}

	const StringConstIter end{std::find_if(m_Current, m_Buffer.cend(), [](auto c) { return !isalnum(c) && c != '_'; })};
	if (end == m_Current) {
		return std::nullopt;
	}

	const CompilerDataTypes::StringView content(m_Current, end);

	const std::optional<Directive> directive{MatchDirective(content)};
	if (!directive.has_value()) {
		const Span                      span{};// TODO: Get the span
		const CompilerDataTypes::String contentStr{content};

		diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_UnknownDirective, contentStr);
		return std::nullopt;
	}

	m_Current = end;
	return directive;
}

bool Preprocessor::TokenizeConstantPrefix(TokenList &tokens, Diagnosis::Vec &diagnoses) {
	ConstantPrefix result;

	switch (*m_Current) {
		case 'L':
			result = ConstantPrefix::L;
			break;
		case 'u':
			result = ConstantPrefix::u;
			break;
		case 'U':
			result = ConstantPrefix::U;
			break;
		default:
			return true;
	}

	++m_Current;

	if (result == ConstantPrefix::u && *m_Current == '8') {
		result = ConstantPrefix::u8;
		++m_Current;
	}

	return TokenizeCharacterConstant(result, tokens, diagnoses);
}

bool Preprocessor::TokenizeCharacterConstant(ConstantPrefix prefix, TokenList &tokens, Diagnosis::Vec &diagnoses) {
	switch (*m_Current) {
		case '\'':// character literal
			++m_Current;
			return TokenizeCharacterOrStringLiteral(prefix, tokens, diagnoses, ConstantType::Character);
		case '"':// string literal
			++m_Current;
			return TokenizeCharacterOrStringLiteral(prefix, tokens, diagnoses, ConstantType::String);
	}

	switch (prefix) {
		case ConstantPrefix::L:
		case ConstantPrefix::u:
		case ConstantPrefix::U:
			--m_Current;
			break;
		case ConstantPrefix::u8:
			m_Current -= 2;
			break;
		default:
			break;
	}

	TokenizeIdentifierOrKeyword(tokens);
	return true;
}

void Preprocessor::TokenizeIdentifierOrKeyword(TokenList &tokens) {
	const StringConstIter end{std::find_if(m_Current, m_Buffer.cend(), [](auto c) { return !isalnum(c) && c != '_'; })};

	if (end == m_Current)
		return;

	const std::string_view content(m_Current, end);

	const std::optional<Keyword> keyword{MatchKeyword(content)};

	m_Current = end;
	if (keyword.has_value()) {
		tokens.emplace_back(keyword.value());
		return;
	}

	tokens.emplace_back(Identifier{std::string{content}});
}

Preprocessor::TokenList Preprocessor::Tokenize(Diagnosis::Vec &diagnoses) {
	TokenList tokens{};

	while (m_Current != m_Buffer.cend()) {
		// skip whitespace
		while (m_Current != m_Buffer.cend() && isspace(*m_Current) && *m_Current != '\n') ++m_Current;

		if (*m_Current == '\n') {
			++m_Current;
			continue;
		}

		if (*m_Current == '#') {
			++m_Current;
			// directives must start on a new line
			const auto potentialDirective{TokenizeDirective(diagnoses)};
			if (potentialDirective.has_value()) {
				tokens.emplace_back(potentialDirective.value());
			} else {
				tokens.emplace_back(TokenizeHash());
			}

			continue;
		}

		// Punctuators
		if (const auto punctuator{TokenizePunctuator(diagnoses)}; punctuator != Punctuator::None) {
			tokens.emplace_back(punctuator);
			continue;
		}

		switch (*m_Current) {
			case 'L':
			case 'u':
			case 'U':
				if (!TokenizeConstantPrefix(tokens, diagnoses))
					return tokens;
				break;
			case '\'':
			case '"':
				if (!TokenizeCharacterConstant(ConstantPrefix::None, tokens, diagnoses))
					return tokens;
				break;
		}

		TokenizeIdentifierOrKeyword(tokens);
	}

	return tokens;
}

Preprocessor::TokenList Preprocessor::Process(Diagnosis::Vec &diagnoses) {
	if (m_Buffer.empty())
		return TokenList{};

	m_Buffer  = std::regex_replace(m_Buffer, std::regex{"\r\n"}, "\n");
	m_Current = m_Buffer.cbegin();

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
        ConstantPrefix prefix, TokenList &tokens, Diagnosis::Vec &diagnoses, Preprocessor::ConstantType type
) {
	if (m_Current == m_Buffer.cend())
		return false;

	bool                      isEscaped{false};
	CompilerDataTypes::String literalContent{};

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

	for (; m_Current != m_Buffer.cend(); ++m_Current) {
		// TODO: move to functor

		const char c{*m_Current};

		if (c == '\\' && !isEscaped) {
			isEscaped = true;
			continue;
		}

		// TODO: We can do away with isEscaped by committing to the escape sequence after the \ character
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
					        m_Current, literalContent, diagnoses, charValueLimit, charValueMask, ValidEscapeBase::Octal
					);
					continue;
				case 'x': {
					if (!TokenizeNumericalEscapeSequence(
					            m_Current, literalContent, diagnoses, charValueLimit, charValueMask,
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
		if (m_Current == m_Buffer.cend() || *m_Current != '"') {
			const Span span{};// TODO: Get the span
			diagnoses.emplace_back(span, Diagnosis::Class::Error, Diagnosis::Kind::PP_StrUnterminated);
			return false;
		}

		tokens.emplace_back(StringConstant{literalContent, prefix});
		return true;
	}
	// TODO: move to helper function

	if (m_Current == m_Buffer.cend() || *m_Current != '\'') {
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
