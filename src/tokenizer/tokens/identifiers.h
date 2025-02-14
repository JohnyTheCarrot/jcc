#ifndef IDENTIFIER_TOKENIZER_H
#define IDENTIFIER_TOKENIZER_H

#include <iosfwd>  // for streampos
#include <optional>// for optional
#include <string>  // for string

#include "misc/Span.h"      // for SpanMarker
#include "tokenizer/token.h"// for Token

namespace jcc::tokenizer {
    class CharIter;
}// namespace jcc::tokenizer

namespace jcc::tokenizer::identifiers {
    [[nodiscard]]
    std::optional<std::size_t>
    CollectRestOfIdentifier(CharIter &charIter, std::string &identifier);

    struct IdentifierTokenStart final {
        std::string   m_Identifier;
        mjolnir::Span m_Span;

        IdentifierTokenStart(std::string identifier, mjolnir::Span span);
    };

    [[nodiscard]]
    Token Tokenize(CharIter const &charIter, IdentifierTokenStart &&tokenStart);
}// namespace jcc::tokenizer::identifiers

#endif//IDENTIFIER_TOKENIZER_H
