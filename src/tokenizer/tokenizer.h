#ifndef JCC_TOKENIZER_H
#define JCC_TOKENIZER_H

#include <exception>// for exception
#include <fstream>  // for basic_ifstream, ifstream
#include <mjolnir/source.hpp>
#include <optional>// for optional
#include <sstream>
#include <string> // for string
#include <variant>// for variant

#include "char_iter.h"// for CharIter
#include "diagnostics/diagnostics.h"
#include "diagnostics/source.h"
#include "misc/Span.h"           // for Span, SpanMarker
#include "tokenizer/token.h"     // for Token
#include "tokenizer_iterator.h"  // for TokenizerIterator
#include "tokens/static_tokens.h"// for StaticTokenTokenizationResult

namespace jcc::tokenizer {
    class TokenizerFileOpenFailure final : public std::exception {};

    class Tokenizer final {
        // Dependent on whether we own the input stream or not.

        std::shared_ptr<diagnostics::Source> m_Source;
        std::istringstream                   m_Input;
        CharIter                             m_CharIter;

        struct TrieTraversalResult final {
            std::variant<Token::Value, std::string> valueOrString{};
            std::size_t                             endPos{};
        };

        bool SkipWhitespace();

        void SkipLineComment();

        void SkipBlockComment(Span &span);

        [[nodiscard]]
        std::optional<Token> TokenizeStaticToken(
                static_tokens::StaticTokenTokenizationResult::
                        ValueOrString const &valueOrString,
                Span &span, bool skippedWhitespace
        );

    public:
        explicit Tokenizer(std::string const &fileName);

        Tokenizer(Tokenizer &&) noexcept;

        Tokenizer &operator=(Tokenizer const &) = delete;

        Tokenizer &operator=(Tokenizer &&) noexcept;

        [[nodiscard]]
        std::optional<Token> GetNextToken();

        [[nodiscard]]
        TokenizerIterator begin();

        [[nodiscard]]
        TokenizerIterator end() const;

        [[nodiscard]]
        Span GetLastSpan() const;

        [[nodiscard]]
        Token SkipUntilConditionEnd();

        [[nodiscard]]
        mjolnir::Source const &GetSource() const noexcept;
    };
}// namespace jcc::tokenizer

#endif//JCC_TOKENIZER_H
