//
// Created by Tuur Martens on 12/01/2024.
//

#ifndef TRIE_H
#define TRIE_H
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

#include <memory>
#include <optional>
#include <string>

template<size_t charRangeStart, size_t charRangeEnd, class TChar, class TValue>
struct TrieNode final {
private:
	// thanks to https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/
	template<size_t N>
	struct StringLiteral {
		constexpr StringLiteral(const TChar (&str)[N]) { std::copy_n(str, N, value); }

		TChar value[N];
	};

	static constexpr bool IsStringViewValid(std::basic_string_view<TChar> key) {
		for (const auto &c: key)
			if (c < charRangeStart || c > charRangeEnd)
				return false;

		return true;
	}

public:
	static_assert(charRangeEnd > charRangeStart, "end of range must be larger than start");
	std::array<std::unique_ptr<TrieNode>, charRangeEnd - charRangeStart + 1> m_Children{};

	struct Leaf {
		TValue m_Value;
	};

	// ~TrieNode() { std::cout << "destructor\n"; }

	std::optional<Leaf> m_Leaf{std::nullopt};

	template<StringLiteral Key>
	void Insert(const TValue &value) {
		static_assert(IsStringViewValid(Key.value), "key contains invalid characters");

		Insert(Key.value, value);
	}

	void Insert(const std::basic_string_view<TChar> &key, const TValue &value) {
		TrieNode *node{this};

		for (size_t keyIdx{}; keyIdx < key.size(); ++keyIdx) {
			const TChar childIndex{static_cast<TChar>(static_cast<size_t>(key[keyIdx]) - charRangeStart)};
			assert(key[keyIdx] >= charRangeStart && key[keyIdx] <= charRangeEnd);

			auto &child{node->m_Children[childIndex]};
			if (child == nullptr) {
				child = std::make_unique<TrieNode>();

				if (keyIdx + 1 == key.size())
					child->m_Leaf = {.m_Value = value};
			}

			node = child.get();
		}
	}

	[[nodiscard]]
	std::optional<TValue> Find(std::basic_istream<TChar> &is, std::size_t &nCharsRead) {
		TrieNode *node{this};
		std::optional<TValue> result{std::nullopt};

		nCharsRead = 0;

		const std::streamoff streamPos{is.tellg()};

		TChar c;
		while (is.get(c)) {
			TrieNode *child;
			const TChar charIdx{static_cast<TChar>(static_cast<size_t>(c) - charRangeStart)};

			if (c < charRangeStart || c > charRangeEnd || (child = node->m_Children[charIdx].get()) == nullptr) {
				is.putback(c);
				break;
			}

			++nCharsRead;

			if (child->m_Leaf.has_value()) {
				result = child->m_Leaf->m_Value;
			}

			node = child;
		}

		if (!result.has_value())
			is.seekg(streamPos, std::istream::beg);// reset stream position
		return result;
	}

	void Clear() { std::fill(m_Children.begin(), m_Children.end(), nullptr); }
};

#endif//TRIE_H
