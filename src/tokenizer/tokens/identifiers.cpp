#include "identifiers.h"

#include <algorithm>// for __move_fn, move
#include <iterator> // for back_insert_iterator, back_inserter
#include <utility>  // for move

#include "misc/Diagnosis.h"     // for Diagnosis, FatalCompilerError
#include "tokenizer/char_iter.h"// for CharIter

namespace jcc::tokenizer::identifiers {
    [[nodiscard]]
    std::optional<std::size_t>
    CollectRestOfIdentifier(CharIter &charIter, std::string &identifier) {
        std::optional<std::size_t> lastSpanMarker{};

        while (charIter != CharIter::c_UntilNewline) {
            auto const [spanMarker, character, isSentinel]{*charIter};

            if (!Identifier::IsValidChar(character))
                break;

            identifier.push_back(character);
            lastSpanMarker = spanMarker;
            ++charIter;
        }

        return lastSpanMarker;
    }

    IdentifierTokenStart::IdentifierTokenStart(
            std::string identifier, mjolnir::Span span
    )
        : m_Identifier{std::move(identifier)}
        , m_Span{span} {
    }

    Token
    Tokenize(CharIter const &charIter, IdentifierTokenStart &&tokenStart) {
        if (charIter == CharIter::end()) {
            Span const span{charIter.GetSource(), tokenStart.m_Span};

            // check if identifier contains a backslash
            if (tokenStart.m_Identifier.find('\\') != std::string::npos)
                // TODO: Universal character names
                // TODO: diagnosis
                throw FatalCompilerError{
                        // Diagnosis::Kind::TODO, std::move(span)
                };
            return Token{
                    .m_Value = Identifier{tokenStart.m_Identifier},
                    .m_Span  = span
            };
        }

        auto &[partialIdentifier, identifierStart]{tokenStart};

        Span span{charIter.GetSource(), tokenStart.m_Span};

        std::string identifier{std::move(partialIdentifier)};

        // check if identifier contains a backslash
        if (identifier.find('\\') != std::string::npos)
            // TODO: Universal character names
            // TODO: diagnosis
            throw FatalCompilerError{
                    // Diagnosis::Kind::TODO, std::move(span)
            };

        Identifier identToken{std::move(identifier)};

        return Token{.m_Value = std::move(identToken), .m_Span = span};
    }
}// namespace jcc::tokenizer::identifiers
