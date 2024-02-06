#include "Diagnosis.h"
#include "fmt/color.h"
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <sstream>

String Diagnosis::ToString() const {
	StringStream ss;

	ss << *this;

	return ss.str();
}

namespace DiagnosticsUtils {
	void OutputLine(OStream &os, int lineNum, const std::string &line) {
		using namespace DiagnosticsUtils;

		std::string lineNrStr{fmt::format(fmt::fg(COLOR_NEUTRAL), "{:>4} ┃", lineNum)};
		os << lineNrStr;

		if (line.empty())
			return;

		os << line << '\n' << fmt::format(fmt::fg(COLOR_NEUTRAL), "     ┖");
	}

	void OutputHighlight(
	        OStream &os, const std::optional<int> &startChar, const std::optional<int> &endChar, int lineLength,
	        Diagnosis::Class diagClass
	) {
		using namespace DiagnosticsUtils;

		const int        actualStartChar{startChar.value_or(0)};
		const int        actualEndChar{endChar.value_or(lineLength)};
		const fmt::color color{GetClassColor(diagClass)};

		if (startChar.has_value()) {
			os << fmt::format(fmt::fg(fmt::color::dim_gray), "{:┄>{}}", "", actualStartChar)
			   << fmt::format(fmt::fg(color), "┠");
		} else {
			os << fmt::format(fmt::fg(color), "┄");
		}

		os << fmt::format(fmt::fg(color), "{:─>{}}", "", std::max(actualEndChar - actualStartChar, 0));

		if (endChar.has_value()) {
			os << fmt::format(fmt::fg(color), "┨");
		} else {
			os << fmt::format(fmt::fg(color), "┄");
		}

		os << '\n';
	}
}// namespace DiagnosticsUtils

OStream &operator<<(OStream &os, const Diagnosis &diagnosis) {
	using namespace DiagnosticsUtils;

	const fmt::color  color{diagnosis.m_Class == Diagnosis::Class::Error ? fmt::color::red : fmt::color::orange};
	const std::string messageKind{diagnosis.m_Class == Diagnosis::Class::Error ? "Error" : "Warning"};

	os << fmt::format(fmt::fg(color), "{}: ", messageKind);

	switch (diagnosis.m_Kind) {
		case Diagnosis::Kind::TK_StrUnterminated:
			os << "String literal was not terminated.";
			break;
		case Diagnosis::Kind::TK_CharUnterminated:
			os << "Character literal was not terminated.";
			break;
		case Diagnosis::Kind::TK_CharNoValue:
			os << "Character literal has no value.";
			break;
		case Diagnosis::Kind::TK_CharOutOfRange:
			os << "Character literal is out of range, its value will be truncated.";
			break;
		case Diagnosis::Kind::TK_CharHexNoDigits:
			os << "Hexadecimal character literal has no digits.";
			break;
		case Diagnosis::Kind::TK_UnknownEscapeSequence:
			os << "Unknown escape sequence '\\" << std::get<char>(diagnosis.m_Data0.value()) << "'.";
			break;
		case Diagnosis::Kind::TK_UnexpectedEOF:
			os << "Unexpected end of file.";
			break;
		case Diagnosis::Kind::TK_PartialTokenEncountered:
			os << fmt::format(
			        "Partial token encountered, did you mean {}?",
			        fmt::styled(std::get<std::string>(diagnosis.m_Data0.value()), fmt::fg(fmt::color::cyan))
			);
			break;
		case Diagnosis::Kind::TK_UnknownDirective:
			os << "Unknown directive '" << std::get<std::string>(diagnosis.m_Data0.value()) << "'.";
			break;
		case Diagnosis::Kind::TK_EscapeExpectedNewline:
			os << "Expected newline after \\.";
			break;
		case Diagnosis::Kind::TK_Unrecognized:
			os << "Unrecognized token '" << std::get<char>(diagnosis.m_Data0.value()) << "'.";
			break;
		case Diagnosis::Kind::TK_InvalidBaseDigit:
			os << "Invalid base digit \'" << std::get<char>(diagnosis.m_Data0.value()) << "'.";
			break;
		case Diagnosis::Kind::TK_UnexpectedIntSuffixChar:
			os << "Unexpected character for integer literal suffix \'" << std::get<char>(diagnosis.m_Data0.value())
			   << "'.";
			break;
		case Diagnosis::Kind::TK_InvalidUniversalCharacterName:
			os << "Invalid universal character name.";
			break;
		case Diagnosis::Kind::TK_IllegalUniversalCharacterName:
			os << "Illegal universal character name.";
			break;
		case Diagnosis::Kind::TK_IllegalBackslash:
			os << "Illegal backslash.";
			break;
	}

	os << "\n\n";

	const Span span{diagnosis.m_Span};

	span.m_IStream.clear();
	span.m_IStream.seekg(span.m_StartPos);

	std::string line;

	if (span.m_Start.m_LineNumber == span.m_End.m_LineNumber) {
		std::getline(span.m_IStream, line);

		OutputLine(os, span.m_Start.m_LineNumber, line);
		OutputHighlight(
		        os, span.m_Start.m_RealCharacterIndex, span.m_End.m_RealCharacterIndex, line.size(), diagnosis.m_Class
		);

		return os;
	}

	std::getline(span.m_IStream, line);

	OutputLine(os, span.m_Start.m_LineNumber, line);
	OutputHighlight(
	        os, span.m_Start.m_RealCharacterIndex, std::nullopt, static_cast<int>(line.size()), diagnosis.m_Class
	);

	for (int lineNum{span.m_Start.m_LineNumber + 1}; lineNum < span.m_End.m_LineNumber; ++lineNum) {
		std::getline(span.m_IStream, line);

		OutputLine(os, lineNum, line);
		OutputHighlight(os, std::nullopt, std::nullopt, static_cast<int>(line.size()), diagnosis.m_Class);
	}

	std::getline(span.m_IStream, line);

	OutputLine(os, span.m_End.m_LineNumber, line);
	OutputHighlight(
	        os, std::nullopt, span.m_End.m_RealCharacterIndex, static_cast<int>(line.size()), diagnosis.m_Class
	);

	return os;
}

std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind) {
	return os << magic_enum::enum_name(kind);
}
