#include "tokenizer/token.h"
#include "misc/compiler_data_types.h"
#include "tokenizer/tokenizer.h"
#include <magic_enum/magic_enum.hpp>

namespace jcc::tokenizer {
	bool IncludeDirective::operator==(IncludeDirective const &other) const {
		return m_Name == other.m_Name;
	}

	void PrintTo(IncludeDirective const &headerName, std::ostream *os) {
		*os << "IncludeDirective(" << magic_enum::enum_name(headerName.m_HeaderType) << ", " << headerName.m_Name
		    << ')';
	}

	bool Identifier::IsValidFirstChar(char c) {
		return std::isalpha(c) || c == '_' || c == '\\';// Backslash allows for universal character names
	}

	bool Identifier::IsValidChar(char c) {
		return std::isalnum(c) || c == '_' || c == '\\';// Backslash allows for universal character names
	}

	bool Identifier::operator==(Identifier const &other) const {
		return m_Name == other.m_Name;
	}

	void PrintTo(Identifier const &identifier, std::ostream *os) {
		*os << "Ident(" << identifier.m_Name << ')';
	}

	bool PpNumber::operator==(PpNumber const &other) const {
		return m_Number == other.m_Number;
	}

	void PrintTo(PpNumber const &ppNumber, std::ostream *os) {
		*os << "PpNumber(" << ppNumber.m_Number << ')';
	}

	bool CharacterConstant::operator==(CharacterConstant const &other) const {
		return m_Character == other.m_Character && m_Prefix == other.m_Prefix;
	}

	void PrintTo(CharacterConstant const &characterConstant, std::ostream *os) {
		if (characterConstant.m_Prefix != ConstantPrefix::None)
			*os << '(' << magic_enum::enum_name(characterConstant.m_Prefix) << ") ";

		if (characterConstant.m_Character <= static_cast<std::uint32_t>(CompilerDataTypeInfo::CHAR::max()) &&
		    characterConstant.m_Character > ' ') {
			*os << '\'' << static_cast<char>(characterConstant.m_Character) << '\'';
			return;
		}

		*os << "U+" << std::hex << characterConstant.m_Character;
	}

	std::string CharacterConstant::ToString() const {
		std::stringstream ss;
		PrintTo(*this, &ss);
		return ss.str();
	}

	bool StringConstant::operator==(StringConstant const &other) const {
		return m_String == other.m_String && m_Prefix == other.m_Prefix;
	}

	std::string StringConstant::ToString() const {
		std::stringstream ss;
		PrintTo(*this, &ss);
		return ss.str();
	}

	bool Token::IsTerminal() const {
		if (!std::holds_alternative<SpecialPurpose>(m_Value))
			return false;

		auto const token{std::get<SpecialPurpose>(m_Value)};

		return token == SpecialPurpose::EndOfFile;
	}

	bool Token::operator==(Token const &other) const {
		return m_Value == other.m_Value && m_Span == other.m_Span;
	}

	void PrintTo(Keyword keyword, std::ostream *os) {
		*os << "Keyword::" << magic_enum::enum_name(keyword);
	}

	void PrintTo(Punctuator punctuator, std::ostream *os) {
		*os << "Punctuator::" << magic_enum::enum_name(punctuator);
	}

	void PrintTo(Directive directive, std::ostream *os) {
		*os << "Directive::" << magic_enum::enum_name(directive);
	}

	void PrintTo(SpecialPurpose specialPurpose, std::ostream *os) {
		*os << "SpecialPurpose::" << magic_enum::enum_name(specialPurpose);
	}

	void PrintTo(StringConstant const &stringConstant, std::ostream *os) {
		if (stringConstant.m_Prefix != ConstantPrefix::None)
			*os << '(' << magic_enum::enum_name(stringConstant.m_Prefix) << ") ";

		*os << stringConstant.m_String;
	}
}// namespace jcc::tokenizer
