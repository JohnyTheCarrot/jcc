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
		CharIter m_CharIter;

		void ExpectNoEof(Span &span) const;

		struct TrieTraversalResult final {
			std::variant<Token::Value, std::string> valueOrString{};
			SpanMarker                              endMarker{};
		};

		[[nodiscard]]
		bool SkipWhitespace();

		void SkipLineComment();

		void SkipBlockComment(Span &span);

	public:
		explicit Tokenizer(std::istream &input, std::string_view fileName);

		[[nodiscard]]
		std::optional<Token> GetNextToken();

		[[nodiscard]]
		TokenizerIterator begin();

		[[nodiscard]]
		TokenizerIterator end();

		[[nodiscard]]
		Span GetLastSpan() const;
	};
}// namespace jcc::tokenizer

#endif//JCC_TOKENIZER_H
