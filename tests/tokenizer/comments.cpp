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
	Diagnosis::Vec diagnoses{};

	Tokenizer tokenizer{"test", iStream};
	EXPECT_TRUE(tokenizer.Tokenize(tokens, diagnoses));

	ASSERT_EQ(tokens.size(), 1);
	EXPECT_EQ(tokens[0]._type, TokenType::KeywordVoid);
	EXPECT_TRUE(diagnoses.empty());
}
