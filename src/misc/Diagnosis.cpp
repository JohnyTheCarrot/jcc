#include "Diagnosis.h"

#include <algorithm>                // for max
#include <cassert>                  // for assert
#include <fmt/color.h>              // for format, fg, styled, color, emph...
#include <format>                   // for format, format_string
#include <magic_enum/magic_enum.hpp>// for enum_name
#include <sstream>                  // for basic_ostream, basic_istream
#include <string_view>              // for basic_string_view, operator<<
#include <utility>                  // for move

#include "fmt/core.h" // for format, format_string
#include "misc/Span.h"// for Span, SpanMarker

namespace jcc {
    enum class Diagnosis::Kind {
        UnexpectedEOF,
        TK_PartialTokenEncountered,
        TK_UnknownDirective,
        TK_EscapeExpectedNewline,
        TK_InvalidBaseDigit,
        TK_UnexpectedIntSuffixChar,
        TK_InvalidUniversalCharacterName,
        TK_IllegalUniversalCharacterName,
        TK_IllegalBackslash,
        TK_ExpectedHeaderName,
        TK_DirectiveNotAloneOnLine,
        TK_UnexpectedChar,
        PP_InclDirectiveFileOpenFailed,
        PP_InclDirectiveExpectedHeaderName,
        PP_DirectiveExpectedNewline,
        PP_MacroExpectedIdentifier,
        PP_MacroExpectedCommaOrRParen,
        PP_MacroEllipsisNotLast,
        PP_MacroExpectedLParen,
        PP_MacroTooFewArgs,
        PP_MacroTooManyArgs,
        PP_IllegalMacroRedefinition,
        PP_IllegalMacroParameterToken,
        PP_UnterminatedMacroParameterList,
        PP_UnterminatedMacroInvocation,
        PP_UnexpectedMacroInvocationArgumentCount,
        PP_MacroDefinedInTermsOfItself,
        PP_HashNotFollowedByParameter,
        PP_DirectiveNotAloneOnLine,
        PP_CondExpectedIdentifier,
        PP_OrphanedConditionalClosure,
        PP_ExpectedEndif,
        PP_UndefExpectedIdentifier,
        PP_Custom,
        PRS_UnrecognizedIntegerSuffix,
        PRS_InvalidFloatingPointLiteral,
        PRS_UnrecognizedFloatingSuffix,
        PRS_InvalidIntegerLiteral,
        PRS_SyntaxError,
        PRS_ExpectedRParen,
        PRS_ExpectedExpressionToFollow,
        PRS_OctalFloatingPoint,
        SEMA_NoCompatibleIntegerType,
        TODO,
    };

    Diagnosis::Diagnosis(
            Span span, Class diagClass, Kind kind, Data &&data0, Data &&data1
    ) noexcept
        : m_Span{std::move(span)}
        , m_Class{diagClass}
        , m_Kind{kind}
        , m_Data0{std::move(data0)}
        , m_Data1{std::move(data1)} {
        std::stringstream ss;
        Print(ss);
        m_Message = ss.str();
    }// namespace jcc

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
    }

    std::string Diagnosis::GetDiagMessage() const {
        switch (m_Kind) {
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
            case Kind::PRS_UnrecognizedFloatingSuffix:
                return "Unrecognized floating point suffix.";
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
            case Kind::PRS_ExpectedExpressionToFollow:
                return "Expected expression to follow.";
            case Kind::PRS_OctalFloatingPoint:
                return "Octal floating point literals are not allowed.";
            case Kind::SEMA_NoCompatibleIntegerType:
                return "No compatible integer type found.";
        }

        assert(false);
        return "You shouldn't be seeing this message. Please report this as a "
               "bug.";
    }

    void Diagnosis::Print(std::ostream &) const {
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

    FatalCompilerError::FatalCompilerError() noexcept = default;

    char const *FatalCompilerError::what() const noexcept {
        return "fatal compiler error";
    }

    std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind) {
        return os << magic_enum::enum_name(kind);
    }
}// namespace jcc
