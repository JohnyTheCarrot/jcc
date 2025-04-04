#include "character_constants.h"

#include <numeric> // for accumulate
#include <optional>// for optional
#include <utility> // for move
#include <vector>  // for vector

#include "diagnostics/variants/tk_pp/char_const_empty.h"
#include "diagnostics/variants/tk_pp/char_const_gt_4_chars.hpp"
#include "diagnostics/variants/tk_pp/multi_byte_char_impl_def.hpp"
#include "diagnostics/variants/tk_pp/utf8_too_many_chars.hpp"
#include "diagnostics/variants/unexpected_eof.hpp"
#include "misc/compiler_data_types.h"// for Char32, Int
#include "misc/Span.h"               // for Span
#include "mjolnir/span.hpp"          // for Span
#include "parsing/parser.h"
#include "tokenizer/char_iter.h"// for CharIter
#include "utils.h"              // for ReadSingleCharacter, ConstantType

namespace jcc::tokenizer::character_constants {
    constexpr int implMaxCharsInCharLiteral{
            sizeof(compiler_data_types::Int::type)
    };

    Token
    Tokenize(CharIter &charIter, ConstantPrefix prefix, std::size_t startPos) {
        auto &compilerState{parsing::CompilerState::GetInstance()};

        if (charIter == CharIter::end()) {
            mjolnir::Span span{startPos, charIter.GetCurrentPos()};

            parsing::CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<diagnostics::UnexpectedEof>(
                            charIter.GetSource(), span
                    );
        }

        // TODO: Char32 is probably not the right type to use here
        std::vector<compiler_data_types::Char32::type> characters{};
        while (true) {
            auto const chOptional{utils::ReadSingleCharacter(
                    charIter, prefix, startPos, utils::ConstantType::Character
            )};
            if (!chOptional.has_value())
                break;

            characters.push_back(chOptional.value());
        }

        Span span{charIter.GetSource(), {startPos, charIter.GetCurrentPos()}};

        if (characters.empty()) {
            std::optional<mjolnir::Span> potentiallyClosingQuote{};
            if (charIter != CharIter::end() && charIter->m_Char == '\'') {
                potentiallyClosingQuote = mjolnir::Span{
                        charIter.GetCurrentPos(), charIter.GetCurrentPos() + 1
                };
            }

            compilerState.EmplaceTemporarilyFatalDiagnostic<
                    diagnostics::CharConstEmpty>(
                    charIter.GetSource(), span.m_Span, potentiallyClosingQuote
            );
        }

        // A UTF-8, UTF-16, or UTF-32 character constant shall not contain more than one character.
        // Notably, this requirement excludes wide character constants.
        if (characters.size() > 1 && prefix != ConstantPrefix::None &&
            prefix != ConstantPrefix::L) {
            compilerState.EmplaceTemporarilyFatalDiagnostic<
                    diagnostics::Utf8TooManyChars>(
                    charIter.GetSource(), span.m_Span
            );
        }

        if (characters.size() > 1) {
            compilerState.EmplaceDiagnostic<diagnostics::MultiByteCharImplDef>(
                    charIter.GetSource(), span.m_Span
            );
        }

        // The value of a multi-character integer character constant is implementation-defined.
        // This compiler allows as many characters as there are bytes in the integer type.
        if (characters.size() > implMaxCharsInCharLiteral) {
            compilerState.EmplaceTemporarilyFatalDiagnostic<
                    diagnostics::CharConstGt4Chars>(
                    charIter.GetSource(), span.m_Span
            );
        }

        auto const result{std::accumulate(
                characters.begin(), characters.end(),
                compiler_data_types::Char32::type{0},
                [](auto acc, auto ch) {
                    return (acc << (sizeof(char) * 8)) | ch;
                }
        )};

        return Token{
                .m_Value =
                        CharacterConstant{
                                .m_Character = result, .m_Prefix = prefix
                        },
                .m_Span = span
        };
    }
}// namespace jcc::tokenizer::character_constants
