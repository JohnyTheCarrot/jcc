#include "../../src/misc/Config.h"
#include "../../src/misc/Tokenizer.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(Tokenizer_SpanCreation, Simple) {
	const String input{C("int")};
	ISStream iStream{input};

	TokenList tokens{};

	Tokenizer tokenizer{iStream};
	tokenizer.Tokenize(tokens);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].m_Type, TokenType::KeywordInt);

	EXPECT_EQ(tokens[0].m_Span.m_LineNumberStart, 1);
	EXPECT_EQ(tokens[0].m_Span.m_LineNumberEnd, 1);
	EXPECT_EQ(tokens[0].m_Span.m_CharacterIndexStart, Tokenizer::FIRST_CHAR_INDEX);
	EXPECT_EQ(tokens[0].m_Span.m_CharacterIndexEnd, Tokenizer::FIRST_CHAR_INDEX + input.size());
}

class LeadingWhitespaceTest : public testing::TestWithParam<std::tuple<String, String, TokenType>> {};

TEST_P(LeadingWhitespaceTest, LeadingSpaces) {
	const String &leadingSpaces{std::get<0>(GetParam())};
	const String &token{std::get<1>(GetParam())};

	const String input{leadingSpaces + token};
	ISStream iStream{input};

	TokenList tokens{};

	Tokenizer tokenizer{iStream};
	tokenizer.Tokenize(tokens);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].m_Type, std::get<2>(GetParam()));

	const auto numNewLines{std::count(leadingSpaces.cbegin(), leadingSpaces.cend(), C('\n'))};
	const auto expectedLineNumber{numNewLines + Tokenizer::FIRST_LINE_INDEX};
	const auto newLinePos{std::find(leadingSpaces.crbegin(), leadingSpaces.crend(), C('\n'))};
	const auto numCharsAfterLastNewLine{std::distance(leadingSpaces.crbegin(), newLinePos)};

	const auto expectedCharacterIndexStart{numCharsAfterLastNewLine + Tokenizer::FIRST_CHAR_INDEX};
	const auto expectedCharacterIndexEnd{expectedCharacterIndexStart + token.size()};

	EXPECT_EQ(tokens[0].m_Span.m_LineNumberStart, expectedLineNumber);
	EXPECT_EQ(tokens[0].m_Span.m_LineNumberEnd, expectedLineNumber);
	EXPECT_EQ(tokens[0].m_Span.m_CharacterIndexStart, expectedCharacterIndexStart);
	EXPECT_EQ(tokens[0].m_Span.m_CharacterIndexEnd, expectedCharacterIndexEnd);
}

INSTANTIATE_TEST_SUITE_P(
		Tokenizer_SpanCreation, LeadingWhitespaceTest,
		testing::Values(
				std::make_tuple(String{C(" ")}, String{C("_Alignas")}, TokenType::KeywordAlignas),
				std::make_tuple(String{C("\t")}, String{C("int")}, TokenType::KeywordInt),
				std::make_tuple(String{C("\t ")}, String{C("volatile")}, TokenType::KeywordVolatile),
				std::make_tuple(String{C("\n")}, String{C("while")}, TokenType::KeywordWhile),
				std::make_tuple(String{C("\n\t")}, String{C("_Alignof")}, TokenType::KeywordAlignof),
				std::make_tuple(String{C("\n\n")}, String{C("_Atomic")}, TokenType::KeywordAtomic),
				std::make_tuple(String{C("\r\n")}, String{C("_Bool")}, TokenType::KeywordBool)
		)
);
