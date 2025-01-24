#include "Diagnosis.h"

#include <algorithm>                // for max
#include <cassert>                  // for assert
#include <fmt/color.h>              // for format, fg, styled, color, emph...
#include <format>                   // for format, format_string
#include <limits>                   // for numeric_limits
#include <magic_enum/magic_enum.hpp>// for enum_name
#include <memory>                   // for __shared_ptr_access, shared_ptr
#include <sstream>                  // for basic_ostream, basic_istream
#include <string_view>              // for basic_string_view, operator<<
#include <utility>                  // for move

#include "fmt/core.h"  // for format, format_string
#include "fmt/format.h"// for buffer::append
#include "misc/Span.h" // for Span, SpanMarker

namespace jcc {
    Diagnosis::Diagnosis(
            Span &&span, Class diagClass, Kind kind, Data &&data0, Data &&data1
    ) noexcept
        : m_Span{std::move(span)}
        , m_Class{diagClass}
        , m_Kind{kind}
        , m_Data0{std::move(data0)}
        , m_Data1{std::move(data1)} {
        std::stringstream ss;
        Print(ss);
        m_Message = ss.str();
    }

    std::string const &Diagnosis::GetMessage() const noexcept {
        return m_Message;
    }

    Diagnosis::Kind Diagnosis::GetKind() const noexcept {
        return m_Kind;
    }

    Diagnosis::Class Diagnosis::GetClass() const noexcept {
        return m_Class;
    }

    void Diagnosis::JumpStreamToStartLine() const {
        m_Span.m_IStream->seekg(std::ios::beg);

        for (int i{1}; i < m_Span.m_Start.m_LineNumber; ++i) {
            m_Span.m_IStream->ignore(
                    std::numeric_limits<std::streamsize>::max(), '\n'
            );
        }
    }

    std::string Diagnosis::GetDiagMessage() const {
        switch (m_Kind) {
            case Kind::TK_StrUnterminated:
                return "String literal was not terminated.";
            case Kind::TK_CharUnterminated:
                return "Character literal was not terminated.";
            case Kind::TK_HeaderNameUnterminated:
                return "Header name was not terminated.";
            case Kind::TK_CharNoValue:
                return "Character literal has no value.";
            case Kind::TK_CharOutOfRange:
                return "Character literal is out of range, its value will "
                       "be truncated.";
            case Kind::TK_CharHexNoDigits:
                return "Hexadecimal character literal has no digits.";
            case Kind::TK_UTFCharMoreThanOneChar:
                return "A UTF-8, UTF-16, or UTF-32 character literal must "
                       "contain exactly one character.";
            case Kind::TK_EscapeSequenceValueTooLarge:
                return "Escape sequence value is too large.";
            case Kind::TK_IntegerCharMoreThanMaxChars:
                return "Character literal has too many characters.";
            case Kind::TK_OctalEscapeSequenceTooLarge:
                return "Octal escape sequence is too large.";
            case Kind::TK_UnknownEscapeSequence:
                return std::format(
                        "Unknown escape sequence '\\{}'.",
                        std::get<char>(m_Data0.value())
                );
            case Kind::UnexpectedEOF:
                return "Unexpected end of file.";
            case Kind::TK_PartialTokenEncountered:
                return fmt::format(
                        fmt::emphasis::bold,
                        "Partial token encountered, did you mean {}?",
                        fmt::styled(
                                std::get<std::string>(m_Data0.value()),
                                fmt::fg(fmt::color::cyan)
                        )
                );
            case Kind::TK_UnknownDirective:
                return std::format(
                        "Unknown directive '{}'.",
                        std::get<std::string>(m_Data0.value())
                );
            case Kind::TK_EscapeExpectedNewline:
                return "Expected newline after \\.";
            case Kind::TK_Unrecognized:
                return std::format(
                        "Unrecognized token '{}'.",
                        std::get<char>(m_Data0.value())
                );
            case Kind::TK_InvalidBaseDigit:
                return std::format(
                        "Invalid base digit \'{}\'.",
                        std::get<char>(m_Data0.value())
                );
            case Kind::TK_UnexpectedIntSuffixChar:
                return std::format(
                        "Unexpected character for integer literal suffix "
                        "\'{}\'.",
                        std::get<char>(m_Data0.value())
                );
            case Kind::TK_InvalidUniversalCharacterName:
                return "Invalid universal character name.";
            case Kind::TK_IllegalUniversalCharacterName:
                return "Illegal universal character name.";
            case Kind::TK_IllegalBackslash:
                return "Illegal backslash.";
            case Kind::TK_ExpectedHeaderName:
                return "Expected header-name.";
            case Kind::TK_DirectiveNotAloneOnLine:
                return "A preprocessor directive must be the only item on "
                       "a line.";
            case Kind::TK_UnexpectedChar:
                return std::format(
                        "Unexpected character '{}'.",
                        std::get<char>(m_Data0.value())
                );
            case Kind::TODO:
                return "Compiler feature unimplemented.";
            case Kind::PP_InclDirectiveFileOpenFailed:
                return "Couldn't open include file.";
            case Kind::PP_MacroExpectedIdentifier:
                return "Expected identifier in macro definition.";
            case Kind::PP_MacroExpectedCommaOrRParen:
                return "Expected comma or right parenthesis.";
            case Kind::PP_MacroEllipsisNotLast:
                return "Variadic macro parameter must be the last "
                       "parameter.";
            case Kind::PP_MacroExpectedLParen:
                return "Expected left parenthesis as part of macro "
                       "invocation.";
            case Kind::PP_MacroTooFewArgs:
                return "Too few arguments passed to function-like macro.";
            case Kind::PP_MacroTooManyArgs:
                return "Too many arguments passed to non-variadic "
                       "function-like macro.";
            case Kind::PP_IllegalMacroRedefinition:
                return "A macro may not be redefined.";
            case Kind::PP_IllegalMacroParameterToken:
                return "Illegal token in macro parameter definitions.";
            case Kind::PP_UnterminatedMacroParameterList:
                return "Macro parameter list was not terminated.";
            case Kind::PP_UnterminatedMacroInvocation:
                return "Function-like macro invocation was not terminated.";
            case Kind::PP_UnexpectedMacroInvocationArgumentCount:
                return "Invalid amount of arguments passed to "
                       "function-like macro.";
            case Kind::PP_MacroDefinedInTermsOfItself:
                return "Macros may not be defined in terms of themselves, "
                       "therefore, this will become an "
                       "identifier or "
                       "keyword upon expansion. Was this intended?";
            case Kind::PP_HashNotFollowedByParameter:
                return "'#' not followed by a macro parameter.";
            case Kind::TK_PreprocessingNumberInvalid:
                return "Invalid preprocessing number.";
            case Kind::PP_DirectiveNotAloneOnLine:
                return "Preprocessor directive must be the only item on a "
                       "line.";
            case Kind::PP_Custom:
                return std::get<std::string>(m_Data0.value());
            case Kind::PP_InclDirectiveExpectedHeaderName:
                return "Expected header name in #include directive.";
            case Kind::PP_DirectiveExpectedNewline:
                return "Expected newline after preprocessor directive.";
            case Kind::PP_CondExpectedIdentifier:
                return "Conditional directive expected identifier.";
            case Kind::PP_OrphanedConditionalClosure:
                return "Closing conditional directive without a matching "
                       "opening directive.";
            case Kind::PP_ExpectedEndif:
                return "#else, #elif, #elifdef or #elifndef may not follow "
                       "#else.";
            case Kind::PP_UndefExpectedIdentifier:
                return "Expected identifier in #undef directive.";
            case Kind::PRS_UnrecognizedIntegerSuffix:
                return "Unrecognized integer suffix.";
            case Kind::PRS_InvalidFloatingPointLiteral:
                return "Invalid floating point literal.";
            case Kind::PRS_InvalidIntegerLiteral:
                return "Invalid integer literal.";
            case Kind::PRS_SyntaxError:
                return std::format(
                        "Syntax error: {}",
                        std::get<std::string>(m_Data0.value())
                );
            case Kind::PRS_ExpectedRParen:
                return "Expected right parenthesis.";
            case Kind::SEMA_NoCompatibleIntegerType:
                return "No compatible integer type found.";
        }

        assert(false);
        return "You shouldn't be seeing this message. Please report this as a "
               "bug.";
    }

    void Diagnosis::Print(std::ostream &ostream) const {
        using namespace diagnostic_utils;

        fmt::color const color{GetClassColor(m_Class)};
        char const      *name{GetClassName(m_Class)};

        ostream << fmt::format(
                fmt::fg(c_ColorNeutral), "In file {}\n",
                fmt::styled(*m_Span.m_FileName, fmt::fg(fmt::color::cyan))
        );
        ostream << fmt::format(fmt::fg(color), "{}: ", name) << GetDiagMessage()
                << "\n\n";

        m_Span.m_IStream->clear();
        auto const streamPos{m_Span.m_IStream->tellg()};
        JumpStreamToStartLine();

        std::string line;
        std::getline(*m_Span.m_IStream, line);

        if (m_Span.m_Start.m_LineNumber == m_Span.m_End.m_LineNumber) {
            OutputLine(ostream, m_Span.m_Start.m_LineNumber, line);
            OutputHighlight(
                    ostream, m_Span.m_Start.m_RealCharacterIndex,
                    m_Span.m_End.m_RealCharacterIndex,
                    static_cast<int>(line.size()), m_Class
            );

            m_Span.m_IStream->seekg(streamPos, std::ios::beg);

            return;
        }

        OutputLine(ostream, m_Span.m_Start.m_LineNumber, line);
        OutputHighlight(
                ostream, m_Span.m_Start.m_RealCharacterIndex, std::nullopt,
                static_cast<int>(line.size()), m_Class
        );

        for (int lineNum{m_Span.m_Start.m_LineNumber + 1};
             lineNum < m_Span.m_End.m_LineNumber; ++lineNum) {
            std::getline(*m_Span.m_IStream, line);

            OutputLine(ostream, lineNum, line);
            OutputHighlight(
                    ostream, std::nullopt, std::nullopt,
                    static_cast<int>(line.size()), m_Class
            );
        }

        std::getline(*m_Span.m_IStream, line);

        OutputLine(ostream, m_Span.m_End.m_LineNumber, line);
        OutputHighlight(
                ostream, std::nullopt, m_Span.m_End.m_RealCharacterIndex,
                static_cast<int>(line.size()), m_Class
        );

        m_Span.m_IStream->seekg(streamPos, std::ios::beg);
    }

    namespace diagnostic_utils {
        void OutputLine(
                std::ostream &ostream, int lineNum, std::string const &line
        ) {
            ostream << fmt::format(fmt::fg(c_ColorNeutral), "{:>4} ┃", lineNum);

            if (line.empty())
                return;

            ostream << fmt::format(
                    "{}\n{}", line,
                    fmt::styled("     ┖", fmt::fg(c_ColorNeutral))
            );
        }

        void OutputHighlight(
                std::ostream &ostream, std::optional<int> const &startChar,
                std::optional<int> const &endChar, int lineLength,
                Diagnosis::Class diagClass
        ) {
            int const        actualStartChar{startChar.value_or(0)};
            int const        actualEndChar{endChar.value_or(lineLength)};
            fmt::color const color{GetClassColor(diagClass)};

            if (startChar.has_value()) {
                ostream << fmt::format(
                        fmt::fg(c_ColorNeutral), "{:┄>{}}", "", actualStartChar
                );
            }

            ostream << fmt::format(
                    fmt::fg(color), "{:─>{}}", "",
                    std::max(actualEndChar - actualStartChar + 1, 1)
            );

            ostream << '\n';
        }
    }// namespace diagnostic_utils

    FatalCompilerError::FatalCompilerError(
            Diagnosis::Kind kind, Span span, OptionalData &&data1,
            OptionalData &&data2
    ) noexcept
        : m_Diagnosis{std::move(span), Diagnosis::Class::Error, kind, std::move(data1), std::move(data2)}
        , m_Message{std::format(
                  "Fatal compiler error: {}", magic_enum::enum_name(kind)
          )} {
    }

    Diagnosis const &FatalCompilerError::GetDiagnosis() const noexcept {
        return m_Diagnosis;
    }

    char const *FatalCompilerError::what() const noexcept {
        return m_Message.c_str();
    }

    std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind) {
        return os << magic_enum::enum_name(kind);
    }
}// namespace jcc
