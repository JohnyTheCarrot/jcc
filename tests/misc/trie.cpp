//
// Created by Tuur Martens on 14/01/2024.
//

#include "../../src/misc/Trie.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(Trie, Insert) {
	constexpr wchar_t charRangeStart{L'!'};
	using Trie = TrieNode<charRangeStart, L'~', wchar_t, int>;
	Trie trie{};
	trie.Insert(L"hoi", 0xdead);
	trie.Insert(L"hi", 0xbeef);

	const auto &hChild{trie.m_children[L'h' - charRangeStart]};
	ASSERT_NE(hChild, nullptr);
	const auto &iChild1{hChild->m_children[L'i' - charRangeStart]};
	ASSERT_NE(iChild1, nullptr);
	EXPECT_EQ(iChild1->m_leaf.value_or(Trie::Leaf{-1}).m_value, 0xbeef);


	const auto &oChild{hChild->m_children[L'o' - charRangeStart]};
	ASSERT_NE(oChild, nullptr);
	const auto &iChild2{oChild->m_children[L'i' - charRangeStart]};
	ASSERT_NE(iChild2, nullptr);
	EXPECT_EQ(iChild2->m_leaf.value_or(Trie::Leaf{-1}).m_value, 0xdead);
}

TEST(Trie, Find_IStream) {
	constexpr wchar_t charRangeStart{L'!'};
	using Trie = TrieNode<charRangeStart, L'~', wchar_t, int>;
	Trie trie{};
	constexpr int value{0x1337};
	trie.Insert(L"_Static_assert", value);

	std::wistringstream iss{L"_Static_assert extra tokens"};
	const auto foundValue{trie.Find(iss)};

	ASSERT_TRUE(foundValue.has_value());
	EXPECT_EQ(foundValue.value(), value);

	std::wstring leftOver{};
	ASSERT_TRUE(std::getline(iss, leftOver, L'\n'));
	EXPECT_EQ(leftOver, L" extra tokens");
}
