#include "utils.h"

#include <cctype>// for isalpha

#include "diagnostics/variants/tk_pp/untermed_char.h"
#include "diagnostics/variants/tk_pp/untermed_string.h"
#include "escape_sequences.h"// for Tokenize
#include "misc/Span.h"       // for Span, SpanMarker (ptr only)
#include "parsing/parser.h"
#include "tokenizer/char_iter.h"// for CharIter

namespace jcc::tokenizer::utils {
    [[nodiscard]]
    std::optional<compiler_data_types::Char32::type> ReadSingleCharacter(
            CharIter &charIter, ConstantPrefix prefix, std::size_t start,
            ConstantType constantType
    ) {
        if (charIter == CharIter::end() || charIter->m_Char == '\n') {
            mjolnir::Span const span{start, charIter.GetCurrentPos()};

            auto &compilerState{parsing::CompilerState::GetInstance()};
            if (constantType == ConstantType::String) {
                compilerState
                        .EmplaceFatalDiagnostic<diagnostics::UntermedString>(
                                charIter.GetSource(), span
                        );
            }

            if (constantType == ConstantType::Character) {
                compilerState.EmplaceFatalDiagnostic<diagnostics::UntermedChar>(
                        charIter.GetSource(), span
                );
            }
        }

        // TODO: UTF-8, UTF-16 and UTF-32
        auto const c{charIter->m_Char};
        ++charIter;

        if (c == '\\') {
            auto const backslashPos{charIter.GetCurrentPos() - 1};

            return escape_sequences::Tokenize(charIter, backslashPos, prefix);
        }

        if (auto const terminator{
                    constantType == ConstantType::Character ? '\'' : '"'
            };
            c == terminator) {
            return std::nullopt;
        }

        return static_cast<compiler_data_types::Char32::type>(c);
    }

    bool IsNonDigit(char c) {
        return std::isalpha(c) || c == '_';
    }
}// namespace jcc::tokenizer::utils
