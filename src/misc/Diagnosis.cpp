//
// Created by tuurm on 04/07/2023.
//

#include "Diagnosis.h"
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <sstream>

String Diagnosis::ToString() const {
	StringStream ss;

	ss << *this;

	return ss.str();
}

OStream &operator<<(OStream &os, const Diagnosis &diagnosis) {
	switch (diagnosis.m_Kind) {
		case Diagnosis::Kind::TK_StrUnterminated:
			os << "String literal was not terminated";
			break;
		case Diagnosis::Kind::TK_CharUnterminated:
			os << "CharStream literal was not terminated";
			break;
		case Diagnosis::Kind::TK_CharNoValue:
			os << "CharStream literal has no value";
			break;
		case Diagnosis::Kind::TK_CharOutOfRange:
			os << "CharStream literal is out of range, its value will be truncated";
			break;
		case Diagnosis::Kind::TK_CharHexNoDigits:
			os << "Hexadecimal character literal has no digits";
			break;
		case Diagnosis::Kind::TK_UnknownEscapeSequence:
			os << "Unknown escape sequence '\\" << std::get<char>(diagnosis.m_Data0.value()) << '\'';
			break;
		case Diagnosis::Kind::TK_UnexpectedEOF:
			os << "Unexpected end of file";
			break;
		case Diagnosis::Kind::TK_PartialTokenEncountered:
			os << "Partial token encountered, did you mean '" << std::get<std::string>(diagnosis.m_Data0.value())
			   << '\'';
			break;
		case Diagnosis::Kind::TK_UnknownDirective:
			os << "Unknown directive '" << std::get<std::string>(diagnosis.m_Data0.value()) << '\'';
			break;
		case Diagnosis::Kind::TK_EscapeExpectedNewline:
			os << "Expected newline after \\";
			break;
		case Diagnosis::Kind::TK_Unrecognized:
			os << "Unrecognized token '" << std::get<char>(diagnosis.m_Data0.value()) << '\'';
			break;
		case Diagnosis::Kind::TK_InvalidBaseDigit:
			os << "Invalid base digit \'" << std::get<char>(diagnosis.m_Data0.value()) << '\'';
			break;
		case Diagnosis::Kind::TK_UnexpectedIntSuffixChar:
			os << "Unexpected character for integer literal suffix \'" << std::get<char>(diagnosis.m_Data0.value())
			   << '\'';
			break;
		case Diagnosis::Kind::TK_InvalidUniversalCharacterName:
			os << "Invalid universal character name";
			break;
		case Diagnosis::Kind::TK_IllegalUniversalCharacterName:
			os << "Illegal universal character name";
			break;
		case Diagnosis::Kind::TK_IllegalBackslash:
			os << "Illegal backslash";
			break;
		case Diagnosis::Kind::TK_LoneCarriageReturn:
			os << "Lone carriage return";
			break;
	}

	os << '\n';

	// os << diagnosis.m_Span.GetTextVersion();

	return os;
}

std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind) {
	return os << magic_enum::enum_name(kind);
}
