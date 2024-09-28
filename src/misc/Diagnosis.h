#ifndef JCC_DIAGNOSIS_H
#define JCC_DIAGNOSIS_H

#include <fmt/color.h>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "Span.h"

namespace jcc {
    struct Diagnosis final {
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
            TODO,
        };

        enum class Class {
            Warning,
            Error,
        };

        Span  m_Span;
        Class m_Class{};
        Kind  m_Kind{};
        Data  m_Data0{}, m_Data1{};

        void PrintDiagMessage() const;

        void Print() const;
    };

    namespace diagnostic_utils {
        constexpr fmt::color c_ColorNeutral{fmt::color::dim_gray};

        constexpr fmt::color GetClassColor(Diagnosis::Class diagClass
        ) noexcept {
            switch (diagClass) {
                case Diagnosis::Class::Warning:
                    return fmt::color::orange;
                case Diagnosis::Class::Error:
                default:
                    return fmt::color::red;
            }
        }

        constexpr char const *GetClassName(Diagnosis::Class diagClass
        ) noexcept {
            switch (diagClass) {
                case Diagnosis::Class::Warning:
                    return "Warning";
                case Diagnosis::Class::Error:
                default:
                    return "Error";
            }
        }

        void OutputLine(int lineNum, std::string const &line);

        void OutputHighlight(
                std::optional<int> const &startChar,
                std::optional<int> const &endChar, int lineLength,
                Diagnosis::Class diagClass
        );
    }// namespace diagnostic_utils

    class FatalCompilerError final : public std::exception {
        using OptionalData = Diagnosis::Data;

        std::string     m_Message;
        Diagnosis::Kind m_Kind;
        Span            m_Span;
        OptionalData    m_Data1;
        OptionalData    m_Data2;

    public:
        FatalCompilerError(
                Diagnosis::Kind kind, Span span,
                OptionalData &&data1 = std::nullopt,
                OptionalData &&data2 = std::nullopt
        ) noexcept;

        [[nodiscard]]
        Span const &GetSpan() const noexcept;

        [[nodiscard]]
        Diagnosis::Kind GetKind() const noexcept;

        [[nodiscard]]
        OptionalData const &GetData1() const noexcept;

        [[nodiscard]]
        OptionalData const &GetData2() const noexcept;

        [[nodiscard]]
        char const *what() const noexcept override;
    };

    std::ostream &operator<<(std::ostream &os, Diagnosis::Kind kind);
}// namespace jcc

#endif//JCC_DIAGNOSIS_H
