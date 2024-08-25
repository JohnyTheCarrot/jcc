#include "Diagnosis.h"

#include <fmt/color.h>
#include <format>
#include <magic_enum/magic_enum.hpp>
#include <utility>

namespace jcc {
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
			case Kind::TK_UTFCharMoreThanOneChar:
				fmt::print("A UTF-8, UTF-16, or UTF-32 character literal must contain exactly one character.");
				break;
			case Kind::TK_EscapeSequenceValueTooLarge:
				fmt::print("Escape sequence value is too large.");
				break;
			case Kind::TK_IntegerCharMoreThanMaxChars:
				fmt::print("Character literal has too many characters.");
				break;
			case Kind::TK_OctalEscapeSequenceTooLarge:
				fmt::print("Octal escape sequence is too large.");
				break;
			case Kind::TK_UnknownEscapeSequence:
				fmt::print("Unknown escape sequence '\\{}'.", std::get<char>(m_Data0.value()));
				break;
			case Kind::UnexpectedEOF:
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
			case Kind::TK_UnexpectedChar:
				fmt::print("Unexpected character '{}'.", std::get<char>(m_Data0.value()));
				break;
			case Kind::TODO:
				fmt::print("Compiler feature unimplemented.");
				break;
			case Kind::PP_InclDirectiveFileOpenFailed:
				fmt::print("Couldn't open include file.");
				break;
			case Kind::PP_MacroExpectedIdentifier:
				fmt::print("Expected identifier in macro definition.");
				break;
			case Kind::PP_MacroExpectedCommaOrRParen:
				fmt::print("Expected comma or right parenthesis.");
				break;
			case Kind::PP_MacroEllipsisNotLast:
				fmt::print("Variadic macro parameter must be the last parameter.");
				break;
			case Kind::PP_MacroExpectedLParen:
				fmt::print("Expected left parenthesis as part of macro invocation.");
				break;
			case Kind::PP_MacroTooFewArgs:
				fmt::print("Too few arguments passed to function-like macro.");
				break;
			case Kind::PP_MacroTooManyArgs:
				fmt::print("Too many arguments passed to non-variadic function-like macro.");
				break;
			case Kind::PP_IllegalMacroRedefinition:
				fmt::print("A macro may not be redefined.");
				break;
			case Kind::PP_IllegalMacroParameterToken:
				fmt::print("Illegal token in macro parameter definitions.");
				break;
			case Kind::PP_UnterminatedMacroParameterList:
				fmt::print("Macro parameter list was not terminated.");
				break;
			case Kind::PP_UnterminatedMacroInvocation:
				fmt::print("Function-like macro invocation was not terminated.");
				break;
			case Kind::PP_UnexpectedMacroInvocationArgumentCount:
				fmt::print("Invalid amount of arguments passed to function-like macro.");
				break;
			case Kind::PP_MacroDefinedInTermsOfItself:
				fmt::print("Macros may not be defined in terms of themselves, therefore, this will become an "
				           "identifier or "
				           "keyword upon expansion. Was this intended?");
				break;
			case Kind::PP_HashNotFollowedByParameter:
				fmt::print("'#' not followed by a macro parameter.");
				break;
			case Kind::TK_PreprocessingNumberInvalid:
				fmt::print("Invalid preprocessing number.");
				break;
		}
	}

	void Diagnosis::Print() const {
		using namespace DiagnosticsUtils;

		fmt::color const color{GetClassColor(m_Class)};
		char const      *name{GetClassName(m_Class)};

		fmt::print(fmt::fg(COLOR_NEUTRAL), "In file {}\n", fmt::styled(*m_Span.m_FileName, fmt::fg(fmt::color::cyan)));
		fmt::print(fmt::fg(color), "{}: ", name);
		PrintDiagMessage();
		fmt::print("\n\n");


		m_Span.m_IStream->clear();
		m_Span.m_IStream->seekg(m_Span.m_StartPos);

		std::string line;

		if (m_Span.m_Start.m_LineNumber == m_Span.m_End.m_LineNumber) {
			std::getline(*m_Span.m_IStream, line);

			OutputLine(m_Span.m_Start.m_LineNumber, line);
			OutputHighlight(
			        m_Span.m_Start.m_RealCharacterIndex, m_Span.m_End.m_RealCharacterIndex,
			        static_cast<int>(line.size()), m_Class
			);

			return;
		}

		std::getline(*m_Span.m_IStream, line);

		OutputLine(m_Span.m_Start.m_LineNumber, line);
		OutputHighlight(m_Span.m_Start.m_RealCharacterIndex, std::nullopt, static_cast<int>(line.size()), m_Class);

		for (int lineNum{m_Span.m_Start.m_LineNumber + 1}; lineNum < m_Span.m_End.m_LineNumber; ++lineNum) {
			std::getline(*m_Span.m_IStream, line);

			OutputLine(lineNum, line);
			OutputHighlight(std::nullopt, std::nullopt, static_cast<int>(line.size()), m_Class);
		}

		std::getline(*m_Span.m_IStream, line);

		OutputLine(m_Span.m_End.m_LineNumber, line);
		OutputHighlight(std::nullopt, m_Span.m_End.m_RealCharacterIndex, static_cast<int>(line.size()), m_Class);
	}

	namespace DiagnosticsUtils {
		void OutputLine(int lineNum, std::string const &line) {
			fmt::print(fmt::fg(COLOR_NEUTRAL), "{:>4} ┃", lineNum);

			if (line.empty())
				return;

			fmt::print("{}\n{}", line, fmt::styled("     ┖", fmt::fg(COLOR_NEUTRAL)));
		}

		void OutputHighlight(
		        std::optional<int> const &startChar, std::optional<int> const &endChar, int lineLength,
		        Diagnosis::Class diagClass
		) {
			int const        actualStartChar{startChar.value_or(0)};
			int const        actualEndChar{endChar.value_or(lineLength)};
			fmt::color const color{GetClassColor(diagClass)};

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

	FatalCompilerError::FatalCompilerError(
	        Diagnosis::Kind kind, Span span, OptionalData &&data1, OptionalData &&data2
	) noexcept
	    : m_Message{std::format("Fatal compiler error: {}", magic_enum::enum_name(kind))}
	    , m_Kind{kind}
	    , m_Span{std::move(span)}
	    , m_Data1{std::move(data1)}
	    , m_Data2{std::move(data2)} {
	}

	Span const &FatalCompilerError::GetSpan() const noexcept {
		return m_Span;
	}

	Diagnosis::Kind FatalCompilerError::GetKind() const noexcept {
		return m_Kind;
	}

	FatalCompilerError::OptionalData const &FatalCompilerError::GetData1() const noexcept {
		return m_Data1;
	}

	FatalCompilerError::OptionalData const &FatalCompilerError::GetData2() const noexcept {
		return m_Data2;
	}

	char const *FatalCompilerError::what() const noexcept {
		return m_Message.c_str();
	}

	std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind) {
		return os << magic_enum::enum_name(kind);
	}
}// namespace jcc
