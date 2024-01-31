//
// Created by tuurm on 04/07/2023.
//

#include "Diagnosis.h"
#include "SpanOld.h"
#include "magic_enum/magic_enum.hpp"
#include <iostream>
#include <sstream>

String Diagnosis::ToString() const {
	StringStream ss;

	ss << *this;

	return ss.str();
}

OStream &operator<<(OStream &os, const Diagnosis &diagnosis) {
	switch (diagnosis.m_Kind) {
		case Diagnosis::Kind::PP_StrUnterminated:
			os << "String literal was not terminated";
			break;
		case Diagnosis::Kind::PP_CharUnterminated:
			os << "CharStream literal was not terminated";
			break;
		case Diagnosis::Kind::PP_CharNoValue:
			os << "CharStream literal has no value";
			break;
		case Diagnosis::Kind::PP_CharOutOfRange:
			os << "CharStream literal is out of range, its value will be truncated";
			break;
		case Diagnosis::Kind::PP_CharHexNoDigits:
			os << "Hexadecimal character literal has no digits";
			break;
		case Diagnosis::Kind::PP_UnknownEscapeSequence:
			os << "Unknown escape sequence '\\" << std::get<char>(diagnosis.m_Data0.value()) << '\'';
			break;
		case Diagnosis::Kind::PP_UnexpectedEOF:
			os << "Unexpected end of file";
			break;
		case Diagnosis::Kind::PP_PartialTokenEncountered:
			os << "Partial token encountered, did you mean '" << std::get<std::string>(diagnosis.m_Data0.value())
			   << '\'';
			break;
		case Diagnosis::Kind::PP_UnknownDirective:
			os << "Unknown directive '" << std::get<std::string>(diagnosis.m_Data0.value()) << '\'';
			break;
		case Diagnosis::Kind::PP_EscapeExpectedNewline:
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
	}

	os << '\n';

	// os << diagnosis.m_Span.GetTextVersion();

	return os;
}

[[deprecated]]
void Path(const std::string &filePath, const SpanOld &span) {
	std::cout << filePath << "(" << span.GetLineNumber() << ':' << span.GetColumn() << "): ";
}

[[deprecated]]
void Warn(const std::string &filePath, std::istream &inputStream, const SpanOld &span, const std::string &message) {
	Path(filePath, span);
	std::cout << "Warning: " << message << std::endl << span.GetTextVersion();
}

[[deprecated]] [[noreturn]]
void Error(const std::string &filePath, std::istream &inputStream, const SpanOld &span, const std::string &message) {
	Path(filePath, span);
	std::cout << "Error: " << message << std::endl << span.GetTextVersion();

	exit(1);
}

std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind) {
	return os << magic_enum::enum_name(kind);
}
