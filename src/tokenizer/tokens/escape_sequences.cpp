#include "escape_sequences.h"

#include <cstdint>                   // for uint64_t
#include <limits>                    // for numer...
#include <misc/compiler_data_types.h>// for Char32
#include <optional>                  // for optional
#include <utility>                   // for move

#include "diagnostics/variants/tk_pp/escape_seq_too_large.hpp"// for Escap...
#include "diagnostics/variants/tk_pp/hex_escape_empty.hpp"    // for HexEs...
#include "diagnostics/variants/tk_pp/unknown_escape_seq.hpp"  // for Unkno...
#include "diagnostics/variants/todo.hpp"                      // for TodoE...
#include "diagnostics/variants/unexpected_eof.hpp"            // for Unexp...
#include "misc/Span.h"                                        // for Span
#include "mjolnir/span.hpp"                                   // for Span
#include "parsing/parser.h"                                   // for Compi...
#include "tokenizer/char_iter.h"                              // for CharIter
#include "tokenizer/token.h"                                  // for GetCo...

namespace jcc::tokenizer::escape_sequences {
    using Character = std::uint8_t;

    [[nodiscard]]
    std::optional<Character> SimpleEscapeSequence(Character c) {
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
    std::optional<Character> GetOctalDigit(Character c) {
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
    Character OctalEscapeSequence(Character firstDigit, CharIter &charIter) {
        constexpr char MAX_OCTAL_DIGIT_BITS{3};

        Character  result{GetOctalDigit(firstDigit).value()};
        auto const addDigit{[&result, &charIter](Character digit) {
            std::uint64_t result64{static_cast<std::uint64_t>(result)};
            result64 <<= MAX_OCTAL_DIGIT_BITS;
            result64 |= digit;

            if (result64 & ~std::numeric_limits<std::uint8_t>::max()) {
                auto &compilerState{parsing::CompilerState::GetInstance()};

                mjolnir::Span span{
                        charIter.GetCurrentPos(), charIter.GetCurrentPos() + 1
                };

                compilerState.EmplaceTemporarilyFatalDiagnostic<
                        diagnostics::EscapeSeqTooLarge>(
                        charIter.GetSource(), span
                );
            }

            result = static_cast<Character>(result64);
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
    int
    HexadecimalEscapeSequence(CharIter &charIter, std::size_t backslashPos) {
        constexpr int MAX_HEX_DIGIT_BITS{4};

        std::uint32_t result{0};
        auto const    addDigit{[&result, &charIter](Character digit) {
            std::uint64_t result64{static_cast<std::uint64_t>(result)};
            result64 <<= MAX_HEX_DIGIT_BITS;
            result64 |= digit;

            if (result64 & ~static_cast<std::uint64_t>(
                                   std::numeric_limits<std::uint32_t>::max()
                           )) {
                auto &compilerState{parsing::CompilerState::GetInstance()};

                mjolnir::Span span{
                        charIter.GetCurrentPos(), charIter.GetCurrentPos() + 1
                };

                compilerState.EmplaceTemporarilyFatalDiagnostic<
                           diagnostics::EscapeSeqTooLarge>(
                        charIter.GetSource(), span
                );
            }

            result = static_cast<std::uint32_t>(result64);
        }};

        auto &compilerState{parsing::CompilerState::GetInstance()};

        if (charIter == CharIter::end()) {
            mjolnir::Span span{backslashPos, charIter.GetCurrentPos()};

            compilerState.EmplaceTemporarilyFatalDiagnostic<
                    diagnostics::HexEscapeEmpty>(charIter.GetSource(), span);
        }

        if (auto const firstDigit{GetHexadecimalDigit(charIter->m_Char)};
            firstDigit.has_value()) {
            result = firstDigit.value();
        } else {
            mjolnir::Span span{backslashPos, charIter.GetCurrentPos()};

            compilerState.EmplaceTemporarilyFatalDiagnostic<
                    diagnostics::HexEscapeEmpty>(charIter.GetSource(), span);
        }

        while (++charIter != CharIter::end()) {
            if (auto const digit{GetHexadecimalDigit(charIter->m_Char)};
                digit.has_value()) {
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
            Span const &span
    ) {
        if (auto const [min, max]{GetConstantPrefixRange(prefix)};
            result < min || result > max) {
            auto &compilerState{parsing::CompilerState::GetInstance()};

            compilerState.EmplaceTemporarilyFatalDiagnostic<
                    diagnostics::EscapeSeqTooLarge>(span.m_Source, span.m_Span);
        }

        return result;
    }

    compiler_data_types::Char32::type
    TokenizeNoSizeCheck(CharIter &charIter, std::size_t backslashPos) {
        auto &compilerState{parsing::CompilerState::GetInstance()};
        if (charIter == CharIter::end()) {
            auto const    currentPos{charIter.GetCurrentPos()};
            mjolnir::Span span{currentPos - 1, currentPos};

            compilerState.EmplaceFatalDiagnostic<diagnostics::UnexpectedEof>(
                    charIter.GetSource(), span
            );
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
            return HexadecimalEscapeSequence(charIter, backslashPos);
        }

        mjolnir::Span span{backslashPos, charIter.GetCurrentPos()};

        if (c == 'u' || c == 'U') {
            // TODO: Universal character names
            compilerState.EmplaceFatalDiagnostic<diagnostics::TodoError>(
                    charIter.GetSource(), span
            );
        }

        compilerState.EmplaceTemporarilyFatalDiagnostic<
                diagnostics::UnknownEscapeSeq>(charIter.GetSource(), span);
    }

    compiler_data_types::Char32::type Tokenize(
            CharIter &charIter, std::size_t backslashPos, ConstantPrefix prefix
    ) {
        auto const result{TokenizeNoSizeCheck(charIter, backslashPos)};
        Span const span{
                charIter.GetSource(), {backslashPos, charIter.GetCurrentPos()}
        };

        return ValidateEscapeSequenceSize(result, prefix, span);
    }
}// namespace jcc::tokenizer::escape_sequences
