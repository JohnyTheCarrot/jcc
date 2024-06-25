#ifndef JCC_TOKENIZER_H
#define JCC_TOKENIZER_H

#include "char_iter.h"
#include "misc/trie.h"
#include "tokenizer/token.h"
#include "tokenizer_iterator.h"
#include <optional>
#include <string_view>

namespace jcc::tokenizer {
	class Tokenizer final {
		using TokenTrieValue = std::variant<Keyword, Directive, Punctuator>;

		using TokenTrie = TrieNode<'!', '~', TokenTrieValue>;
		static TokenTrie m_TokenTrie;

		std::shared_ptr<std::string> m_FileName;
		SpanMarker                   m_SubTokenSpanStart{};
		SpanMarker                   m_TokenSpanStart{};
		std::streamoff               m_CurrentTokenLineStart{};
		std::istream                *m_Input;
		CharIter                     m_CharIter;

		[[nodiscard]]
		std::optional<SpanMarker> CollectRestOfIdentifier(std::string &identifier);

		struct TrieTraversalResult final {
			std::variant<Token::Value, std::string> valueOrString{};
			SpanMarker                              endMarker{};
		};

		[[nodiscard]]
		TrieTraversalResult TraverseTrie();

		[[nodiscard]]
		Token CollectIdentifier(
		        std::string_view partialIdentifier, SpanMarker identifierStart, std::streampos const &identifierStartPos
		);

		[[nodiscard]]
		bool SkipWhitespace();

		[[nodiscard]]
		static Token::Value TrieResultToTokenValue(TokenTrieValue value);

	public:
		explicit Tokenizer(std::istream &input, std::string_view fileName);

		[[nodiscard]]
		std::optional<Token> GetNextToken();

		[[nodiscard]]
		TokenizerIterator begin();

		[[nodiscard]]
		TokenizerIterator end();
	};
}// namespace jcc::tokenizer

#endif//JCC_TOKENIZER_H
