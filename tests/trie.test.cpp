#include "misc/trie.h"

#include <sstream>

#include <gtest/gtest.h>

using namespace jcc;

TEST(Trie, Insert) {
    constexpr char charRangeStart{'!'};
    using Trie = TrieNode<charRangeStart, '~', int>;
    Trie trie{};
    trie.Insert("hoi", 0xdead);
    trie.Insert("hi", 0xbeef);

    auto const &hChild{trie.m_Children['h' - charRangeStart]};
    ASSERT_NE(hChild, nullptr);
    auto const &iChild1{hChild->m_Children['i' - charRangeStart]};
    ASSERT_NE(iChild1, nullptr);
    EXPECT_EQ(iChild1->m_Leaf.value_or(Trie::Leaf{-1}).m_Value, 0xbeef);


    auto const &oChild{hChild->m_Children['o' - charRangeStart]};
    ASSERT_NE(oChild, nullptr);
    auto const &iChild2{oChild->m_Children['i' - charRangeStart]};
    ASSERT_NE(iChild2, nullptr);
    EXPECT_EQ(iChild2->m_Leaf.value_or(Trie::Leaf{-1}).m_Value, 0xdead);
}

TEST(Trie, Find_IStreamExtraTrailingTokens) {
    constexpr char charRangeStart{'!'};
    using Trie = TrieNode<charRangeStart, '~', int>;
    Trie              trie{};
    constexpr int     value{0x1337};
    std::string const key{"_Static_assert"};
    trie.Insert(key, value);

    std::istringstream iss{"_Static_assert extra tokens"};
    std::size_t        nCharsRead{};
    auto const         foundValue{trie.Find(iss, nCharsRead)};

    ASSERT_TRUE(foundValue.has_value());
    EXPECT_EQ(foundValue.value(), value);
    EXPECT_EQ(nCharsRead, key.size());

    std::string leftOver{};
    ASSERT_TRUE(std::getline(iss, leftOver, '\n'));
    EXPECT_EQ(leftOver, " extra tokens");
}

TEST(Trie, Find_IStreamSimple) {
    constexpr char charRangeStart{'!'};
    using Trie = TrieNode<charRangeStart, '~', int>;
    Trie              trie{};
    constexpr int     value{0x123};
    std::string const key{"_Static_assert"};
    trie.Insert(key, value);

    std::istringstream iss{key};
    std::size_t        nCharsRead{};
    auto const         foundValue{trie.Find(iss, nCharsRead)};

    ASSERT_TRUE(foundValue.has_value());
    EXPECT_EQ(foundValue.value(), value);
    EXPECT_EQ(nCharsRead, key.size());
}
