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
        PP_MacroExpectedLParen,
        PP_MacroTooFewArgs,
        PP_MacroTooManyArgs,
        PRS_UnrecognizedIntegerSuffix,
        PRS_InvalidFloatingPointLiteral,
        PRS_UnrecognizedFloatingSuffix,
        PRS_InvalidIntegerLiteral,
        PRS_ExpectedRParen,
        SEMA_NoCompatibleIntegerType,
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
            case Kind::PP_MacroExpectedLParen:
                return "Expected left parenthesis as part of macro "
                       "invocation.";
            case Kind::PP_MacroTooFewArgs:
                return "Too few arguments passed to function-like macro.";
            case Kind::PP_MacroTooManyArgs:
                return "Too many arguments passed to non-variadic "
                       "function-like macro.";
            case Kind::PRS_UnrecognizedIntegerSuffix:
                return "Unrecognized integer suffix.";
            case Kind::PRS_UnrecognizedFloatingSuffix:
                return "Unrecognized floating point suffix.";
            case Kind::PRS_InvalidFloatingPointLiteral:
                return "Invalid floating point literal.";
            case Kind::PRS_InvalidIntegerLiteral:
                return "Invalid integer literal.";
            case Kind::PRS_ExpectedRParen:
                return "Expected right parenthesis.";
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
