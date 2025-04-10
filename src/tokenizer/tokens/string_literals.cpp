#include "string_literals.h"

#include <limits>  // for numeric_limits
#include <optional>// for optional
#include <string>  // for string
#include <utility> // for move

#include "diagnostics/variants/unexpected_eof.hpp"// for UnexpectedEof
#include "misc/Span.h"                            // for Span
#include "mjolnir/span.hpp"                       // for Span
#include "parsing/parser.h"                       // for CompilerState
#include "tokenizer/char_iter.h"                  // for CharIter
#include "utils.h"                                // for ReadSingleCharacter

namespace jcc::tokenizer::string_literals {
    Token
    Tokenize(CharIter &charIter, ConstantPrefix prefix, std::size_t startPos) {
        if (charIter == CharIter::end()) {
            mjolnir::Span span{startPos, charIter.GetCurrentPos()};

            parsing::CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<diagnostics::UnexpectedEof>(
                            charIter.GetSource(), span
                    );
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
                    charIter, prefix, startPos, utils::ConstantType::String
            )};
            if (!chOptional.has_value())
                break;

            strContents.push_back(static_cast<char>(
                    chOptional.value() & std::numeric_limits<char>::max()
            ));
        }

        Span span{charIter.GetSource(), {startPos, charIter.GetCurrentPos()}};

        return Token{
                .m_Value = StringConstant{std::move(strContents), prefix},
                .m_Span  = std::move(span)
        };
    }
}// namespace jcc::tokenizer::string_literals
