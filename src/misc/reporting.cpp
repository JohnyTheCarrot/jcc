//
// Created by tuurm on 04/07/2023.
//

#include "reporting.h"
#include "Span.h"
#include <iostream>
#include <sstream>

std::string Diagnosis::ToString() const {
	std::stringstream ss;

	switch (kind_) {
		case Kind::TK_Unrecognized:
			ss << "Unrecognized token '" << std::get<char>(data0_.value()) << '\'';
			break;
		case Kind::TK_UnknownEscapeSequence:
			ss << "Unknown escape sequence '\\" << std::get<char>(data0_.value()) << '\'';
			break;
		case Kind::TK_StrUnterminated:
			ss << "String literal was not terminated";
			break;
		case Kind::TK_InvalidBaseDigit:
			ss << "Invalid base digit \'" << std::get<char>(data0_.value()) << '\'';
			break;
		case Kind::TK_UnexpectedIntSuffixChar:
			ss << "Unexpected character for integer literal suffix \'" << std::get<char>(data0_.value()) << '\'';
			break;
	}

	ss << std::endl;

	return ss.str();
}

void Path(const std::string &filePath, const Span &span) {
	std::cout << filePath << "(" << span.GetLineNumber() << ':' << span.GetColumn() << "): ";
}

void Warn(const std::string &filePath, std::istream &inputStream, const Span &span, const std::string &message) {
	Path(filePath, span);
	std::cout << "Warning: " << message << std::endl << span.GetTextVersion();
}

[[noreturn]]
void Error(const std::string &filePath, std::istream &inputStream, const Span &span, const std::string &message) {
	Path(filePath, span);
	std::cout << "Error: " << message << std::endl << span.GetTextVersion();

	exit(1);
}
