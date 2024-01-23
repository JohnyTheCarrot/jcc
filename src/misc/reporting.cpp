//
// Created by tuurm on 04/07/2023.
//

#include "reporting.h"
#include "SpanOld.h"
#include <iostream>
#include <sstream>

std::string Diagnosis::ToString() const {
	std::stringstream ss;

	ss << *this;

	return ss.str();
}

std::ostream &operator<<(std::ostream &os, const Diagnosis &diagnosis) {
	switch (diagnosis.kind_) {
		case Diagnosis::Kind::TK_Unrecognized:
			os << "Unrecognized token '" << std::get<char>(diagnosis.data0_.value()) << '\'';
			break;
		case Diagnosis::Kind::TK_UnknownEscapeSequence:
			os << "Unknown escape sequence '\\" << std::get<char>(diagnosis.data0_.value()) << '\'';
			break;
		case Diagnosis::Kind::TK_StrUnterminated:
			os << "String literal was not terminated";
			break;
		case Diagnosis::Kind::TK_InvalidBaseDigit:
			os << "Invalid base digit \'" << std::get<char>(diagnosis.data0_.value()) << '\'';
			break;
		case Diagnosis::Kind::TK_UnexpectedIntSuffixChar:
			os << "Unexpected character for integer literal suffix \'" << std::get<char>(diagnosis.data0_.value())
			   << '\'';
			break;
	}

	os << '\n';

	os << diagnosis.span_.GetTextVersion();

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
