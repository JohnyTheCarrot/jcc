#include "character_constants.h"

#include <numeric>
#include <utility>
#include <vector>

#include "misc/Diagnosis.h"
#include "tokenizer/char_iter.h"
#include "utils.h"

namespace jcc::tokenizer::character_constants {
    constexpr int implMaxCharsInCharLiteral{
            sizeof(compiler_data_types::Int::type)
    };

    Token Tokenize(
            CharIter &charIter, ConstantPrefix prefix,
            SpanMarker const &startMarker
    ) {
        if (charIter == CharIter::end()) {
            Span span{
                    charIter.GetFileName(), startMarker,
                    charIter.GetCurrentSpanMarker(),
                    charIter.GetInput()->tellg(), charIter.GetInput()
            };

            throw FatalCompilerError{
                    Diagnosis::Kind::UnexpectedEOF, std::move(span)
            };
        }

        std::vector<compiler_data_types::Char32::type> characters{};
        while (true) {
            auto const chOptional{utils::ReadSingleCharacter(
                    charIter, prefix, startMarker,
                    utils::ConstantType::Character
            )};
            if (!chOptional.has_value())
                break;

            characters.push_back(chOptional.value());
        }

        Span span{
                charIter.GetFileName(), startMarker,
                charIter.GetCurrentSpanMarker(), charIter.GetInput()->tellg(),
                charIter.GetInput()
        };

        if (characters.empty()) {
            throw FatalCompilerError{
                    Diagnosis::Kind::TK_CharNoValue, std::move(span)
            };
        }

        // A UTF-8, UTF-16, or UTF-32 character constant shall not contain more than one character.
        // Notably, this requirement excludes wide character constants.
        if (characters.size() > 1 && prefix != ConstantPrefix::None &&
            prefix != ConstantPrefix::L) {
            throw FatalCompilerError{
                    Diagnosis::Kind::TK_UTFCharMoreThanOneChar, std::move(span)
            };
        }

        // The value of a multi-character integer character constant is implementation-defined.
        // This compiler allows as many characters as there are bytes in the integer type.
        if (characters.size() > implMaxCharsInCharLiteral) {
            throw FatalCompilerError{
                    Diagnosis::Kind::TK_IntegerCharMoreThanMaxChars,
                    std::move(span)
            };
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
                .m_Span = std::move(span)
        };
    }
}// namespace jcc::tokenizer::character_constants
