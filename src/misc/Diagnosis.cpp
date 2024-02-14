#include "Diagnosis.h"
#include <fmt/color.h>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

void Diagnosis::PrintDiagMessage() const {
	switch (m_Kind) {
		case Kind::TK_StrUnterminated:
			fmt::print("String literal was not terminated.");
			break;
		case Kind::TK_CharUnterminated:
			fmt::print("Character literal was not terminated.");
			break;
		case Kind::TK_HeaderNameUnterminated:
			fmt::print("Header name was not terminated.");
			break;
		case Kind::TK_CharNoValue:
			fmt::print("Character literal has no value.");
			break;
		case Kind::TK_CharOutOfRange:
			fmt::print("Character literal is out of range, its value will be truncated.");
			break;
		case Kind::TK_CharHexNoDigits:
			fmt::print("Hexadecimal character literal has no digits.");
			break;
		case Kind::TK_UnknownEscapeSequence:
			fmt::print("Unknown escape sequence '\\{}'.", std::get<char>(m_Data0.value()));
		case Kind::TK_UnexpectedEOF:
			fmt::print("Unexpected end of file.");
			break;
		case Kind::TK_PartialTokenEncountered:
			fmt::print(
			        fmt::emphasis::bold, "Partial token encountered, did you mean {}?",
			        fmt::styled(std::get<std::string>(m_Data0.value()), fmt::fg(fmt::color::cyan))
			);
			break;
		case Kind::TK_UnknownDirective:
			fmt::print("Unknown directive '{}'.", std::get<std::string>(m_Data0.value()));
			break;
		case Kind::TK_EscapeExpectedNewline:
			fmt::print("Expected newline after \\.");
			break;
		case Kind::TK_Unrecognized:
			fmt::print("Unrecognized token '{}'.", std::get<char>(m_Data0.value()));
			break;
		case Kind::TK_InvalidBaseDigit:
			fmt::print("Invalid base digit \'{}\'.", std::get<char>(m_Data0.value()));
			break;
		case Kind::TK_UnexpectedIntSuffixChar:
			fmt::print("Unexpected character for integer literal suffix \'{}\'.", std::get<char>(m_Data0.value()));
			break;
		case Kind::TK_InvalidUniversalCharacterName:
			fmt::print("Invalid universal character name.");
			break;
		case Kind::TK_IllegalUniversalCharacterName:
			fmt::print("Illegal universal character name.");
			break;
		case Kind::TK_IllegalBackslash:
			fmt::print("Illegal backslash.");
			break;
		case Kind::TK_ExpectedHeaderName:
			fmt::print("Expected header-name.");
			break;
		case Kind::TK_DirectiveNotAloneOnLine:
			fmt::print("A preprocessor directive must be the only item on a line.");
			break;
	}
}

void Diagnosis::Print() const {
	using namespace DiagnosticsUtils;

	const fmt::color color{GetClassColor(m_Class)};
	const char      *name{GetClassName(m_Class)};

	fmt::print(fmt::fg(color), "{}: ", name);
	PrintDiagMessage();
	fmt::print("\n\n");


	m_Span.m_IStream.clear();
	m_Span.m_IStream.seekg(m_Span.m_StartPos);

	std::string line;

	if (m_Span.m_Start.m_LineNumber == m_Span.m_End.m_LineNumber) {
		std::getline(m_Span.m_IStream, line);

		OutputLine(m_Span.m_Start.m_LineNumber, line);
		OutputHighlight(
		        m_Span.m_Start.m_RealCharacterIndex, m_Span.m_End.m_RealCharacterIndex, static_cast<int>(line.size()),
		        m_Class
		);

		return;
	}

	std::getline(m_Span.m_IStream, line);

	OutputLine(m_Span.m_Start.m_LineNumber, line);
	OutputHighlight(m_Span.m_Start.m_RealCharacterIndex, std::nullopt, static_cast<int>(line.size()), m_Class);

	for (int lineNum{m_Span.m_Start.m_LineNumber + 1}; lineNum < m_Span.m_End.m_LineNumber; ++lineNum) {
		std::getline(m_Span.m_IStream, line);

		OutputLine(lineNum, line);
		OutputHighlight(std::nullopt, std::nullopt, static_cast<int>(line.size()), m_Class);
	}

	std::getline(m_Span.m_IStream, line);

	OutputLine(m_Span.m_End.m_LineNumber, line);
	OutputHighlight(std::nullopt, m_Span.m_End.m_RealCharacterIndex, static_cast<int>(line.size()), m_Class);
}

namespace DiagnosticsUtils {
	void OutputLine(int lineNum, const std::string &line) {
		fmt::print(fmt::fg(COLOR_NEUTRAL), "{:>4} ┃", lineNum);

		if (line.empty())
			return;

		fmt::print("{}\n{}", line, fmt::styled("     ┖", fmt::fg(COLOR_NEUTRAL)));
	}

	void OutputHighlight(
	        const std::optional<int> &startChar, const std::optional<int> &endChar, int lineLength,
	        Diagnosis::Class diagClass
	) {
		const int        actualStartChar{startChar.value_or(0)};
		const int        actualEndChar{endChar.value_or(lineLength)};
		const fmt::color color{GetClassColor(diagClass)};

		if (startChar.has_value()) {
			fmt::print(fmt::fg(COLOR_NEUTRAL), "{:┄>{}}", "", actualStartChar);
			fmt::print(fmt::fg(color), "┠");
		} else {
			fmt::print(fmt::fg(color), "┄");
		}

		fmt::print(fmt::fg(color), "{:─>{}}", "", std::max(actualEndChar - actualStartChar, 1));

		if (endChar.has_value()) {
			fmt::print(fmt::fg(color), "┨");
		} else {
			fmt::print(fmt::fg(color), "┄");
		}

		fmt::print("\n");
	}
}// namespace DiagnosticsUtils

std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind) {
	return os << magic_enum::enum_name(kind);
}
