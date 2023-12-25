#include "../../src/misc/tokenizer.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(Tokenizer_Misc, UnknownToken) {
	const std::string input{"$"};
	std::istringstream iStream{input};

	TokenList tokens{};
	Diagnosis::Vec diagnoses{};

	Tokenizer tokenizer{"test", iStream};
	const bool result{tokenizer.Tokenize(tokens, diagnoses)};

	ASSERT_FALSE(diagnoses.empty());
	EXPECT_TRUE(tokens.empty());
	EXPECT_FALSE(result);
	EXPECT_EQ(diagnoses[0].class_, Diagnosis::Class::Error);
	EXPECT_EQ(diagnoses[0].kind_, Diagnosis::Kind::TK_Unrecognized);
}
