#ifndef CHAR_ITER_H
#define CHAR_ITER_H

#include "misc/Span.h"

#include <string_view>
#include <variant>

namespace jcc::tokenizer {
	struct CharInfo final {
		SpanMarker m_SpanMarker{};
		char       m_Char{};
		bool       m_IsSentinel{false};

		[[nodiscard]]
		bool
		operator==(CharInfo const &other) const;

		friend void PrintTo(CharInfo const &charInfo, std::ostream *os);
	};

	class CharIter final {
		struct Sentinel final {
			SpanMarker m_LastSpanMarker{};
		};

		using CurrentChar = std::variant<CharInfo, Sentinel>;

		std::istream                *m_Input{};
		std::shared_ptr<std::string> m_FileName{};
		CurrentChar                  m_CurrentChar{Sentinel{}};
		SpanMarker                   m_CurrentSpanMarker{};

		[[nodiscard]]
		bool Next();

		CharInfo Expect(char c);

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type        = CharInfo;
		using pointer           = value_type const *;
		using difference_type   = int;

		CharIter();

		explicit CharIter(std::istream &input, std::string_view fileName);

		explicit CharIter(char until);

		[[nodiscard]]
		std::istream *GetInput() const;

		[[nodiscard]]
		std::shared_ptr<std::string> const &GetFileName() const;

		[[nodiscard]]
		Sentinel const &GetSentinel() const;

		[[nodiscard]]
		SpanMarker const &GetCurrentSpanMarker() const;

		[[nodiscard]]
		value_type
		operator*() const;

		[[nodiscard]]
		pointer
		operator->() const;

		CharIter &operator++();

		CharIter operator++(int);

		[[nodiscard]]
		CharIter begin() const;

		static CharIter end();

		[[nodiscard]]
		bool
		operator==(CharIter const &other) const;

		[[nodiscard]]
		bool
		operator!=(CharIter const &other) const;

		static CharIter const c_UntilNewline;
	};

	static_assert(std::input_iterator<CharIter>);
}// namespace jcc::tokenizer

#endif//CHAR_ITER_H
