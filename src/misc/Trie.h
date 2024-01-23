//
// Created by Tuur Martens on 12/01/2024.
//

#ifndef TRIE_H
#define TRIE_H
#include <array>
#include <cassert>
#include <iostream>

#include <memory>
#include <optional>
#include <string>

template<size_t charRangeStart, size_t charRangeEnd, class TChar, class TValue>
struct TrieNode final {
	static_assert(charRangeEnd > charRangeStart, "end of range must be larger than start");
	std::array<std::unique_ptr<TrieNode>, charRangeEnd - charRangeStart> m_children{};

	struct Leaf {
		TValue m_value;
	};

	// ~TrieNode() { std::cout << "destructor\n"; }

	std::optional<Leaf> m_leaf{std::nullopt};

	void Insert(const std::basic_string<TChar> &key, const TValue &value) {
		TrieNode *node{this};

		for (size_t keyIdx{}; keyIdx < key.size(); ++keyIdx) {
			const TChar childIndex{static_cast<TChar>(static_cast<size_t>(key[keyIdx]) - charRangeStart)};
			assert(key[keyIdx] >= charRangeStart && key[keyIdx] <= charRangeEnd);

			auto &child{node->m_children[childIndex]};
			if (child == nullptr) {
				child = std::make_unique<TrieNode>();

				if (keyIdx + 1 == key.size())
					child->m_leaf = {.m_value = value};
			}

			node = child.get();
		}
	}

	[[nodiscard]]
	std::optional<TValue> Find(std::basic_istream<TChar> &is) {
		TrieNode *node{this};
		std::optional<TValue> result{std::nullopt};

		TChar c;
		while (is.get(c)) {
			TrieNode *child;
			const TChar charIdx{static_cast<TChar>(static_cast<size_t>(c) - charRangeStart)};

			if (charIdx < charRangeStart || charIdx > charRangeEnd ||
				(child = node->m_children[charIdx].get()) == nullptr) {
				is.putback(c);
				return result;
			}

			if (child->m_leaf.has_value()) {
				result = child->m_leaf->m_value;
			}

			node = child;
		}

		return std::nullopt;
	}

	void Clear() {
		for (auto &child: m_children) { child = nullptr; }
	}
};

#endif//TRIE_H
