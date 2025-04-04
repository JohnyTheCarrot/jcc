#include "identifiers.h"

#include <utility>// for move

#include "diagnostics/variants/todo.hpp"// for TodoError
#include "misc/Span.h"                  // for Span
#include "parsing/parser.h"             // for CompilerState
#include "tokenizer/char_iter.h"        // for CharIter

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
            if (tokenStart.m_Identifier.find('\\') != std::string::npos) {
                // TODO: Universal character names
                parsing::CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                                std::move(span.m_Source), span.m_Span
                        );
            }
            return Token{
                    .m_Value = Identifier{tokenStart.m_Identifier},
                    .m_Span  = span
            };
        }

        auto &[partialIdentifier, identifierStart]{tokenStart};

        Span span{charIter.GetSource(), tokenStart.m_Span};

        std::string identifier{std::move(partialIdentifier)};

        // check if identifier contains a backslash
        if (identifier.find('\\') != std::string::npos) {
            // TODO: Universal character names
            parsing::CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                            std::move(span.m_Source), span.m_Span
                    );
        }

        Identifier identToken{std::move(identifier)};

        return Token{.m_Value = std::move(identToken), .m_Span = span};
    }
}// namespace jcc::tokenizer::identifiers
