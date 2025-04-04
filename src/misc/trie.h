#ifndef TRIE_H
#define TRIE_H

#include <algorithm>
#include <array>
#include <cassert>
#include <memory>
#include <optional>
#include <sstream>
#include <utility>

namespace jcc {
    // TODO: memory inefficient, refactor to use an unordered_map instead of a std::array
    template<char charRangeStart, char charRangeEnd, class TValue>
    struct TrieNode final {
    public:
        static_assert(
                charRangeEnd > charRangeStart,
                "end of range must be larger than start"
        );
        std::array<std::unique_ptr<TrieNode>, charRangeEnd - charRangeStart + 1>
                m_Children{};

        struct Leaf {
            TValue m_Value;
        };

        using Value = TValue;

        std::optional<Leaf> m_Leaf{std::nullopt};

        TrieNode() = default;

        TrieNode(std::initializer_list<std::pair<std::string_view, TValue>>
                         initList) {
            for (auto const &[key, value] : initList) Insert(key, value);
        }

        void Insert(std::string_view key, TValue const &value) {
            TrieNode *node{this};

            for (size_t keyIdx{}; keyIdx < key.size(); ++keyIdx) {
                char const childIndex{static_cast<char>(
                        static_cast<size_t>(key[keyIdx]) - charRangeStart
                )};
                assert(key[keyIdx] >= static_cast<int>(charRangeStart) &&
                       key[keyIdx] <= static_cast<int>(charRangeEnd));

                auto &child{node->m_Children[childIndex]};
                if (child == nullptr) {
                    child = std::make_unique<TrieNode>();

                    if (keyIdx + 1 == key.size())
                        child->m_Leaf = {.m_Value = value};
                }

                node = child.get();
            }
        }

        // [[nodiscard]]
        // std::optional<TValue> Find(tokenizer::CharIter &charIter) const {
        // 	TrieNode const       *node{this};
        // 	std::optional<TValue> result{std::nullopt};
        //
        // 	while (node != nullptr) {
        // 		if (charIter == tokenizer::CharIter::end())
        // 			return result;
        //
        // 		auto const [spanMarker, character, isSentinel]{*charIter};
        // 		char const charIdx{static_cast<char>(static_cast<size_t>(character) - charRangeStart)};
        //
        // 		if (charIdx < 0 || charIdx > m_Children.size() - 1)
        // 			return result;
        //
        // 		auto const &child{node->m_Children[charIdx].get()};
        //
        // 		if (child == nullptr)
        // 			return result;
        //
        // 		if (child->m_Leaf.has_value()) {
        // 			result = child->m_Leaf->m_Value;
        // 		}
        //
        // 		node = child;
        // 		++charIter;
        // 	}
        //
        // 	return result;
        // }

        [[nodiscard]]
        std::pair<TrieNode const *, std::optional<Leaf>> Find(char character
        ) const {
            char const charIdx{static_cast<char>(
                    static_cast<size_t>(character) - charRangeStart
            )};

            if (charIdx < 0 ||
                charIdx > static_cast<int>(m_Children.size() - 1))
                return {nullptr, std::nullopt};

            auto const &child{m_Children[charIdx].get()};

            if (child == nullptr)
                return {nullptr, std::nullopt};

            return {child, child->m_Leaf};
        }

        [[nodiscard]]
        std::optional<TValue>
        Find(std::istringstream &is, std::size_t &nCharsRead) const {
            TrieNode const       *node{this};
            std::optional<TValue> result{std::nullopt};

            nCharsRead = 0;

            std::streamoff const streamPos{is.tellg()};

            char c;
            while (is.get(c)) {
                TrieNode  *child;
                char const charIdx{static_cast<char>(
                        static_cast<size_t>(c) - charRangeStart
                )};

                if (c < charRangeStart || c > charRangeEnd ||
                    (child = node->m_Children[charIdx].get()) == nullptr) {
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
            std::ranges::fill(m_Children, nullptr);
        }
    };
}// namespace jcc
#endif//TRIE_H
