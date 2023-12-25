#include "../../src/misc/tokenizer.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <ios>
#include <sstream>
#include <variant>

struct IntLiteralTestValidInput {
	int inputNumber;
	IntegerLiteralType type;
	enum class Notation { Dec, Hex, Oct } notation;
	bool isUnsigned;
};

class IntLiteralTest : public testing::TestWithParam<IntLiteralTestValidInput> {};

TEST_P(IntLiteralTest, ValidDecimalIntSigned) {
	auto input{GetParam()};
	std::stringstream inputTextStream;

	switch (input.notation) {
		case IntLiteralTestValidInput::Notation::Hex:
			inputTextStream << "0x" << std::hex;
			break;
		case IntLiteralTestValidInput::Notation::Oct:
			inputTextStream << '0' << std::oct;
			break;
		default:
			break;
	}

	inputTextStream << input.inputNumber << std::dec;

	if (input.isUnsigned)
		inputTextStream << 'u';

	switch (input.type) {
		case IntegerLiteralType::Long:
			inputTextStream << 'l';
			break;
		case IntegerLiteralType::LongLong:
			inputTextStream << "ll";
			break;
		default:
			break;
	}

	std::istringstream iStream{inputTextStream.str()};

	TokenList tokens{};
	Diagnosis::Vec diagnoses{};
	Tokenizer tokenizer{"test", iStream};

	EXPECT_TRUE(tokenizer.Tokenize(tokens, diagnoses));
	EXPECT_TRUE(diagnoses.empty());
	ASSERT_FALSE(tokens.empty());
	ASSERT_TRUE(std::holds_alternative<IntegerLiteralTokenValue>(tokens[0]._value));
	auto tokenValue{std::get<IntegerLiteralTokenValue>(tokens[0]._value)};

	EXPECT_EQ(tokens[0]._type, TokenType::IntegerLiteral);
	EXPECT_EQ(tokenValue.value, input.inputNumber);
	EXPECT_EQ(tokenValue.isUnsigned, input.isUnsigned);
	EXPECT_EQ(tokenValue.type, input.type);
}

TEST(Tokenizer_IntLiteral, InvalidBaseDigit) {
	const std::string input{"0xZ"};
	std::istringstream iStream{input};

	TokenList tokens{};
	Diagnosis::Vec diagnoses{};

	Tokenizer tokenizer{"test", iStream};
	EXPECT_FALSE(tokenizer.Tokenize(tokens, diagnoses));

	EXPECT_TRUE(tokens.empty());
	ASSERT_FALSE(diagnoses.empty());
	EXPECT_EQ(diagnoses[0].class_, Diagnosis::Class::Error);
	EXPECT_EQ(diagnoses[0].kind_, Diagnosis::Kind::TK_InvalidBaseDigit);
}

constexpr IntLiteralTestValidInput testInputs[] = {
		{12345, IntegerLiteralType::Int, IntLiteralTestValidInput::Notation::Dec, false},
		{42, IntegerLiteralType::Int, IntLiteralTestValidInput::Notation::Dec, true},
		{32, IntegerLiteralType::Int, IntLiteralTestValidInput::Notation::Dec, false},
		{0xDEAD, IntegerLiteralType::LongLong, IntLiteralTestValidInput::Notation::Hex, false}};

INSTANTIATE_TEST_SUITE_P(Hexadecimal, IntLiteralTest, testing::ValuesIn(testInputs));
