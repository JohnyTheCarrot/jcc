#include <gtest/gtest.h>
#include <misc/Tokenizer.h>
#include <misc/config.h>

TEST(Tokenizer_Tokens, Simple) {
	const String input{C("volatile")};
	ISStream     iStream{input};

	TokenList tokens{};

	Tokenizer tokenizer{iStream};
	tokenizer.Tokenize(tokens);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0].m_Type, TokenType::KeywordVolatile);
}
