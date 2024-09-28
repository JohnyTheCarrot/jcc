#include "escape_sequences.h"

#include <misc/compiler_data_types.h>
#include <utility>

#include "misc/Diagnosis.h"

namespace jcc::tokenizer::escape_sequences {
    [[nodiscard]]
    std::optional<char> SimpleEscapeSequence(char c) {
        switch (c) {
            case '\'':
            case '"':
            case '?':
            case '\\':
                return c;
            case 'a':
                return '\a';
            case 'b':
                return '\b';
            case 'f':
                return '\f';
            case 'n':
                return '\n';
            case 'r':
                return '\r';
            case 't':
                return '\t';
            case 'v':
                return '\v';
            default:
                return std::nullopt;
        }
    }

    [[nodiscard]]
    std::optional<char> GetOctalDigit(char c) {
        switch (c) {
            case '0':
                return '\0';
            case '1':
                return '\1';
            case '2':
                return '\2';
            case '3':
                return '\3';
            case '4':
                return '\4';
            case '5':
                return '\5';
            case '6':
                return '\6';
            case '7':
                return '\7';
            default:
                return std::nullopt;
        }
    }

    [[nodiscard]]
    char OctalEscapeSequence(char firstDigit, CharIter &charIter) {
        constexpr char MAX_OCTAL_DIGIT_BITS{3};

        char       result{GetOctalDigit(firstDigit).value()};
        auto const addDigit{[&result](char digit) {
            result <<= MAX_OCTAL_DIGIT_BITS;
            result |= digit;
        }};

        if (charIter == CharIter::end()) {
            return 0;
        }

        if (auto const secondDigit{GetOctalDigit(charIter->m_Char)};
            secondDigit.has_value()) {
            addDigit(secondDigit.value());
            ++charIter;
        } else {
            return result;
        }

        if (charIter == CharIter::end()) {
            return result;
        }

        if (auto const thirdDigit{GetOctalDigit(charIter->m_Char)};
            thirdDigit.has_value()) {
            addDigit(thirdDigit.value());
            ++charIter;
        }

        return result;
    }

    [[nodiscard]]
    std::optional<int> GetHexadecimalDigit(char c) {
        switch (c) {
            case '0':
                return 0x00;
            case '1':
                return 0x01;
            case '2':
                return 0x02;
            case '3':
                return 0x03;
            case '4':
                return 0x04;
            case '5':
                return 0x05;
            case '6':
                return 0x06;
            case '7':
                return 0x07;
            case '8':
                return 0x08;
            case '9':
                return 0x09;
            case 'a':
            case 'A':
                return 0x0A;
            case 'b':
            case 'B':
                return 0x0B;
            case 'c':
            case 'C':
                return 0x0C;
            case 'd':
            case 'D':
                return 0x0D;
            case 'e':
            case 'E':
                return 0x0E;
            case 'f':
            case 'F':
                return 0x0F;
            default:
                return std::nullopt;
        }
    }

    [[nodiscard]]
    int HexadecimalEscapeSequence(
            CharIter &charIter, SpanMarker const &backslashMarker
    ) {
        constexpr int MAX_HEX_DIGIT_BITS{4};

        int        result{0};
        auto const addDigit{[&result](int digit) {
            result <<= MAX_HEX_DIGIT_BITS;
            result |= digit;
        }};

        if (charIter == CharIter::end()) {
            Span span{
                    charIter.GetFileName(), backslashMarker,
                    charIter.GetCurrentSpanMarker(),
                    charIter.GetInput()->tellg(), charIter.GetInput()
            };

            throw FatalCompilerError{
                    Diagnosis::Kind::UnexpectedEOF, std::move(span)
            };
        }

        if (auto const firstDigit{GetHexadecimalDigit(charIter->m_Char)};
            firstDigit.has_value()) {
            result = firstDigit.value();
        } else {
            Span span{
                    charIter.GetFileName(), backslashMarker,
                    charIter.GetCurrentSpanMarker(),
                    charIter.GetInput()->tellg(), charIter.GetInput()
            };

            throw FatalCompilerError{
                    Diagnosis::Kind::TK_CharHexNoDigits, std::move(span),
                    charIter->m_Char
            };
        }

        int digitIdx{0};
        while (++charIter != CharIter::end()) {
            constexpr int N_INT_HEX_DIGITS{8};

            if (auto const digit{GetHexadecimalDigit(charIter->m_Char)};
                digit.has_value()) {
                // We check for equal to because we already added the first digit
                if (++digitIdx == N_INT_HEX_DIGITS) {
                    Span span{
                            charIter.GetFileName(), backslashMarker,
                            charIter.GetCurrentSpanMarker(),
                            charIter.GetInput()->tellg(), charIter.GetInput()
                    };

                    throw FatalCompilerError{
                            Diagnosis::Kind::TK_EscapeSequenceValueTooLarge,
                            std::move(span)
                    };
                }

                addDigit(digit.value());
            } else {
                break;
            }
        }

        return result;
    }

    [[nodiscard]]
    compiler_data_types::Char32::type ValidateEscapeSequenceSize(
            compiler_data_types::Char32::type result, ConstantPrefix prefix,
            Span &span
    ) {
        if (auto const [min, max]{GetConstantPrefixRange(prefix)};
            result < min || result > max)
            throw FatalCompilerError{
                    Diagnosis::Kind::TK_EscapeSequenceValueTooLarge,
                    std::move(span)
            };

        return result;
    }

    compiler_data_types::Char32::type
    TokenizeNoSizeCheck(CharIter &charIter, SpanMarker const &backslashMarker) {
        if (charIter == CharIter::end()) {
            SpanMarker const &currentMarker{charIter.GetCurrentSpanMarker()};
            Span              span{
                    charIter.GetFileName(), currentMarker, currentMarker,
                    charIter.GetInput()->tellg(), charIter.GetInput()
            };

            throw FatalCompilerError{
                    Diagnosis::Kind::UnexpectedEOF, std::move(span)
            };
        }

        char const c{charIter->m_Char};
        ++charIter;

        if (auto const simpleEscapeSequenceResult{SimpleEscapeSequence(c)};
            simpleEscapeSequenceResult.has_value()) {
            return simpleEscapeSequenceResult.value();
        }

        if (c >= '0' && c <= '7') {
            return OctalEscapeSequence(c, charIter);
        }

        if (c == 'x') {
            return HexadecimalEscapeSequence(charIter, backslashMarker);
        }

        Span span{
                charIter.GetFileName(), backslashMarker,
                charIter.GetCurrentSpanMarker(), charIter.GetInput()->tellg(),
                charIter.GetInput()
        };

        if (c == 'u' || c == 'U') {
            throw FatalCompilerError{Diagnosis::Kind::TODO, std::move(span)};
        }

        throw FatalCompilerError{
                Diagnosis::Kind::TK_UnknownEscapeSequence, std::move(span), c
        };
    }

    compiler_data_types::Char32::type Tokenize(
            CharIter &charIter, SpanMarker const &backslashMarker,
            ConstantPrefix prefix
    ) {
        auto const result{TokenizeNoSizeCheck(charIter, backslashMarker)};
        Span       span{
                charIter.GetFileName(), backslashMarker,
                charIter.GetCurrentSpanMarker(), charIter.GetInput()->tellg(),
                charIter.GetInput()
        };

        return ValidateEscapeSequenceSize(result, prefix, span);
    }
}// namespace jcc::tokenizer::escape_sequences
