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

	std::pair<compiler_data_types::Char32::type, compiler_data_types::Char32::type>
	GetConstantPrefixRange(ConstantPrefix prefix) {
		switch (prefix) {
			case ConstantPrefix::None:
			case ConstantPrefix::u8:
				return std::pair{compiler_data_types::Char::min(), compiler_data_types::Char::max()};
			case ConstantPrefix::L:
				return std::pair{compiler_data_types::WChar::min(), compiler_data_types::WChar::max()};
			case ConstantPrefix::u:
				return std::pair{compiler_data_types::Char16::min(), compiler_data_types::Char16::max()};
			case ConstantPrefix::U:
				return std::pair{compiler_data_types::Char32::min(), compiler_data_types::Char32::max()};
			default:
				assert(false);
				return {};
		}
	}

	ConstantPrefix ToConstantPrefix(std::string_view prefix) {
		if (prefix == "L")
			return ConstantPrefix::L;

		if (prefix == "u")
			return ConstantPrefix::u;

		if (prefix == "U")
			return ConstantPrefix::U;

		if (prefix == "u8")
			return ConstantPrefix::u8;

		return ConstantPrefix::None;
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

		if (characterConstant.m_Character <= static_cast<std::uint32_t>(compiler_data_types::Char::max()) &&
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

	Token::Type Token::GetValueType() const {
		if (std::holds_alternative<Identifier>(m_Value))
			return GenericType::Identifier;

		if (std::holds_alternative<PpNumber>(m_Value))
			return GenericType::PpNumber;

		if (std::holds_alternative<CharacterConstant>(m_Value))
			return GenericType::ChararacterConstant;

		if (std::holds_alternative<StringConstant>(m_Value))
			return GenericType::StringConstant;

		if (std::holds_alternative<Punctuator>(m_Value))
			return std::get<Punctuator>(m_Value);

		if (std::holds_alternative<Keyword>(m_Value))
			return std::get<Keyword>(m_Value);

		if (std::holds_alternative<Directive>(m_Value))
			return std::get<Directive>(m_Value);

		if (std::holds_alternative<SpecialPurpose>(m_Value))
			return std::get<SpecialPurpose>(m_Value);

		assert(false);
		return SpecialPurpose::Error;
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

	void PrintTo(ConstantPrefix constantPrefix, std::ostream *os) {
		*os << "CharacterConstant::" << magic_enum::enum_name(constantPrefix);
	}

	void PrintTo(StringConstant const &stringConstant, std::ostream *os) {
		if (stringConstant.m_Prefix != ConstantPrefix::None)
			*os << '(' << magic_enum::enum_name(stringConstant.m_Prefix) << ") ";

		*os << stringConstant.m_String;
	}

	std::ostream &operator<<(std::ostream &os, Token const &token) {
		os << '[';
		PrintTo(token.m_Span, &os);
		os << "]: ";

		if (std::holds_alternative<IncludeDirective>(token.m_Value)) {
			PrintTo(std::get<IncludeDirective>(token.m_Value), &os);
		} else if (std::holds_alternative<Identifier>(token.m_Value)) {
			PrintTo(std::get<Identifier>(token.m_Value), &os);
		} else if (std::holds_alternative<CharacterConstant>(token.m_Value)) {
			PrintTo(std::get<CharacterConstant>(token.m_Value), &os);
		} else if (std::holds_alternative<StringConstant>(token.m_Value)) {
			PrintTo(std::get<StringConstant>(token.m_Value), &os);
		} else if (std::holds_alternative<Punctuator>(token.m_Value)) {
			os << magic_enum::enum_name(std::get<Punctuator>(token.m_Value));
		} else if (std::holds_alternative<Keyword>(token.m_Value)) {
			os << magic_enum::enum_name(std::get<Keyword>(token.m_Value));
		} else if (std::holds_alternative<Directive>(token.m_Value)) {
			os << magic_enum::enum_name(std::get<Directive>(token.m_Value));
		} else if (std::holds_alternative<SpecialPurpose>(token.m_Value)) {
			os << magic_enum::enum_name(std::get<SpecialPurpose>(token.m_Value));
		} else if (std::holds_alternative<PpNumber>(token.m_Value)) {
			PrintTo(std::get<PpNumber>(token.m_Value), &os);
		} else {
			assert(false);
		}

		return os;
	}
}// namespace jcc::tokenizer
