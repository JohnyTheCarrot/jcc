#ifndef JCC_DIAGNOSIS_H
#define JCC_DIAGNOSIS_H

#include <exception>  // for exception
#include <fmt/color.h>// for color
#include <iosfwd>     // for ostream
#include <optional>   // for nullopt, optional
#include <string>     // for string
#include <variant>    // for variant
#include <vector>     // for vector

#include "Span.h"// for Span

namespace jcc {
    class Diagnosis final {
    public:
        using Vec  = std::vector<Diagnosis>;
        using Data = std::optional<std::variant<char, std::string>>;

        enum class Kind {
            TK_StrUnterminated,
            TK_CharUnterminated,
            TK_HeaderNameUnterminated,
            TK_CharNoValue,
            TK_CharOutOfRange,
            TK_CharHexNoDigits,
            TK_UTFCharMoreThanOneChar,
            TK_IntegerCharMoreThanMaxChars,
            TK_OctalEscapeSequenceTooLarge,
            TK_EscapeSequenceValueTooLarge,
            TK_UnknownEscapeSequence,
            TK_PreprocessingNumberInvalid,
            UnexpectedEOF,
            TK_PartialTokenEncountered,
            TK_UnknownDirective,
            TK_EscapeExpectedNewline,
            TK_Unrecognized,
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
            PRS_InvalidIntegerLiteral,
            SEMA_NoCompatibleIntegerType,
            TODO,
        };

        enum class Class {
            Warning,
            Error,
        };

        Diagnosis(
                Span &&span, Class diagClass, Kind kind,
                Data &&data0 = std::nullopt, Data &&data1 = std::nullopt
        ) noexcept;

        [[nodiscard]]
        std::string const &GetMessage() const noexcept;

        [[nodiscard]]
        Kind GetKind() const noexcept;

        [[nodiscard]]
        Class GetClass() const noexcept;

    private:
        Span        m_Span;
        Class       m_Class{};
        Kind        m_Kind{};
        Data        m_Data0{}, m_Data1{};
        std::string m_Message;

        void JumpStreamToStartLine() const;

        [[nodiscard]]
        std::string GetDiagMessage() const;

        void Print(std::ostream &ostream) const;
    };

    namespace diagnostic_utils {
        constexpr auto c_ColorNeutral{fmt::color::dim_gray};

        constexpr fmt::color
        GetClassColor(Diagnosis::Class diagClass) noexcept {
            switch (diagClass) {
                case Diagnosis::Class::Warning:
                    return fmt::color::orange;
                case Diagnosis::Class::Error:
                default:
                    return fmt::color::red;
            }
        }

        constexpr char const *
        GetClassName(Diagnosis::Class diagClass) noexcept {
            switch (diagClass) {
                case Diagnosis::Class::Warning:
                    return "Warning";
                case Diagnosis::Class::Error:
                default:
                    return "Error";
            }
        }

        void
        OutputLine(std::ostream &ostream, int lineNum, std::string const &line);

        void OutputHighlight(
                std::ostream &ostream, std::optional<int> const &startChar,
                std::optional<int> const &endChar, int lineLength,
                Diagnosis::Class diagClass
        );
    }// namespace diagnostic_utils

    class FatalCompilerError final : public std::exception {
        using OptionalData = Diagnosis::Data;

        Diagnosis   m_Diagnosis;
        std::string m_Message;

    public:
        FatalCompilerError(
                Diagnosis::Kind kind, Span span,
                OptionalData &&data1 = std::nullopt,
                OptionalData &&data2 = std::nullopt
        ) noexcept;

        [[nodiscard]]
        Diagnosis const &GetDiagnosis() const noexcept;

        [[nodiscard]]
        char const *what() const noexcept override;
    };

    std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind);
}// namespace jcc

#endif//JCC_DIAGNOSIS_H
