#include "../../src/misc/tokenizer.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(Tokenizer_StringLiterals, WarnForUnknownEscapeSequence) {
	const std::string strContent{"hi \\j"};
	const std::string input{"\"" + strContent + "\""};
	std::istringstream iStream{input};

	TokenList tokens{};
	Diagnosis::Vec diagnoses{};

	Tokenizer tokenizer{"test", iStream};
	EXPECT_TRUE(tokenizer.Tokenize(tokens, diagnoses));

	ASSERT_FALSE(tokens.empty());
	ASSERT_EQ(tokens[0]._type, TokenType::StringLiteral);
	EXPECT_STREQ(std::get<std::string>(tokens[0]._value).c_str(), strContent.c_str());

	ASSERT_FALSE(diagnoses.empty());
	EXPECT_EQ(diagnoses[0].kind_, Diagnosis::Kind::TK_UnknownEscapeSequence);
	EXPECT_EQ(diagnoses[0].class_, Diagnosis::Class::Warning);
}

TEST(Tokenizer_StringLiterals, ErrorForUnterminated) {
	const std::string input{"\"unterminated"};
	std::istringstream iStream{input};

	TokenList tokens{};
	Diagnosis::Vec diagnoses{};

	Tokenizer tokenizer{"test", iStream};
	EXPECT_FALSE(tokenizer.Tokenize(tokens, diagnoses));

	EXPECT_TRUE(tokens.empty());

	ASSERT_FALSE(diagnoses.empty());
	EXPECT_EQ(diagnoses[0].kind_, Diagnosis::Kind::TK_StrUnterminated);
	EXPECT_EQ(diagnoses[0].class_, Diagnosis::Class::Error);
}
