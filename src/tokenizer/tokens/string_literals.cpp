#include "string_literals.h"

#include <algorithm>// for __copy_fn, copy
#include <iterator> // for back_insert_iterator, back_inserter
#include <optional> // for optional
#include <string>   // for basic_string, string, u32string
#include <utility>  // for move

#include "misc/Diagnosis.h"     // for Diagnosis, FatalCompilerError
#include "misc/Span.h"          // for Span, SpanMarker (ptr only)
#include "tokenizer/char_iter.h"// for CharIter
#include "utils.h"              // for ReadSingleCharacter, ConstantType

namespace jcc::tokenizer::string_literals {
    Token Tokenize(
            CharIter &charIter, ConstantPrefix prefix,
            SpanMarker const &startMarker
    ) {
        if (charIter == CharIter::end()) {
            Span span{
                    charIter.GetFileName(), startMarker,
                    charIter.GetCurrentSpanMarker(), charIter.GetInput()
            };

            throw FatalCompilerError{
                    Diagnosis::Kind::UnexpectedEOF, std::move(span)
            };
        }

        // std::u32string u32Characters{};
        // while (true) {
        //     auto const chOptional{utils::ReadSingleCharacter(
        //             charIter, prefix, startMarker, utils::ConstantType::String
        //     )};
        //     if (!chOptional.has_value())
        //         break;
        //
        //     u32Characters.push_back(chOptional.value());
        // }
        //
        // // TODO: this probably doesn't actually work
        // std::string strContents{};
        // std::ranges::copy(u32Characters, std::back_inserter(strContents));
        // TODO
        std::string strContents{};
        while (true) {
            auto const chOptional{utils::ReadSingleCharacter(
                    charIter, prefix, startMarker, utils::ConstantType::String
            )};
            if (!chOptional.has_value())
                break;

            strContents.push_back(static_cast<char>(
                    chOptional.value() & std::numeric_limits<char>::max()
            ));
        }

        Span span{
                charIter.GetFileName(), startMarker,
                charIter.GetCurrentSpanMarker(), charIter.GetInput()
        };

        return Token{
                .m_Value = StringConstant{std::move(strContents), prefix},
                .m_Span  = std::move(span)
        };
    }
}// namespace jcc::tokenizer::string_literals
