//
// Created by Tuur Martens on 12/01/2024.
//

#ifndef TRIE_H
#define TRIE_H
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>

#include "CharStream.h"
#include <memory>
#include <optional>
#include <string>
#include <variant>

template<size_t charRangeStart, size_t charRangeEnd, class TValue>
struct TrieNode final {
private:
	// thanks to https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/
	template<size_t N>
	struct StringLiteral {
		constexpr StringLiteral(const char (&str)[N]) {
			std::copy_n(str, N, value);
		}

		char value[N];
	};

	static constexpr bool IsStringViewValid(std::basic_string_view<char> key) {
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

	using Value = TValue;

	std::optional<Leaf> m_Leaf{std::nullopt};

	TrieNode() = default;

	TrieNode(std::initializer_list<std::pair<std::basic_string_view<char>, TValue>> initList) {
		for (const auto &[key, value]: initList) Insert(key, value);
	}

	template<StringLiteral Key>
	void Insert(const TValue &value) {
		static_assert(IsStringViewValid(Key.value), "key contains invalid characters");

		Insert(Key.value, value);
	}

	void Insert(const std::basic_string_view<char> &key, const TValue &value) {
		TrieNode *node{this};

		for (size_t keyIdx{}; keyIdx < key.size(); ++keyIdx) {
			const char childIndex{static_cast<char>(static_cast<size_t>(key[keyIdx]) - charRangeStart)};
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

	const TrieNode *GetNode(const CharStream &charStream) const {
		if (!charStream.Good())
			return nullptr;

		const char c{charStream.Get()};
		const char childIndex{static_cast<char>(static_cast<size_t>(c) - charRangeStart)};
		if (childIndex < 0 || childIndex > charRangeEnd - charRangeStart)
			return nullptr;

		return m_Children[childIndex].get();
	}

	[[nodiscard]]
	std::optional<TValue> Find(std::istringstream &is, std::size_t &nCharsRead) const {
		const TrieNode       *node{this};
		std::optional<TValue> result{std::nullopt};

		nCharsRead = 0;

		const std::streamoff streamPos{is.tellg()};

		char c;
		while (is.get(c)) {
			TrieNode  *child;
			const char charIdx{static_cast<char>(static_cast<size_t>(c) - charRangeStart)};

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

	void Clear() {
		std::fill(m_Children.begin(), m_Children.end(), nullptr);
	}
};

#endif//TRIE_H
