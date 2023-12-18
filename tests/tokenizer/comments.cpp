#include "../../src/misc/tokenizer.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(Tokenizer_Comments, LineComment) {
	const std::string input{R"(
    // void
    void
  )"};
	std::istringstream iStream{input};

	TokenList tokens{};

	Tokenizer tokenizer{"test", iStream};
	tokenizer.Tokenize(tokens);

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0]._type, TokenType::KeywordVoid);
}
