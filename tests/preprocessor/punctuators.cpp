#include <gtest/gtest.h>
#include <misc/Config.h>
#include <misc/Preprocessor.h>
#include <misc/reporting.h>
#include <tuple>
#include <variant>

class PunctuatorTest : public testing::TestWithParam<std::tuple<String, std::vector<Preprocessor::Punctuator>>> {};

TEST_P(PunctuatorTest, Punctuators) {
	Preprocessor preprocessor{std::get<0>(GetParam())};

	Diagnosis::Vec diagnoses{};
	const Preprocessor::TokenList tokenList{preprocessor.Process(diagnoses)};
	const auto expectedPunctuators{std::get<1>(GetParam())};
	Preprocessor::TokenList expectedPunctuatorTokenList{};
	std::transform(
	        expectedPunctuators.cbegin(), expectedPunctuators.cend(), std::back_inserter(expectedPunctuatorTokenList),
	        [](auto punctuator) { return Preprocessor::Token{punctuator}; }
	);

	ASSERT_GE(tokenList.size(), 1);
	EXPECT_EQ(expectedPunctuatorTokenList, tokenList);
}

using Punctuator = Preprocessor::Punctuator;
using PunctVec   = std::vector<Punctuator>;

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_Punctuators, PunctuatorTest,
        testing::Values(
                std::make_tuple(String{C("[")}, PunctVec{Punctuator::LeftBracket}),
                std::make_tuple(String{C("]")}, PunctVec{Punctuator::RightBracket}),
                std::make_tuple(String{C("(")}, PunctVec{Punctuator::LeftParenthesis}),
                std::make_tuple(String{C(")")}, PunctVec{Punctuator::RightParenthesis}),
                std::make_tuple(String{C("{")}, PunctVec{Punctuator::LeftBrace}),
                std::make_tuple(String{C("}")}, PunctVec{Punctuator::RightBrace}),
                std::make_tuple(String{C(".")}, PunctVec{Punctuator::Dot}),
                std::make_tuple(String{C("->")}, PunctVec{Punctuator::Arrow}),
                std::make_tuple(String{C("++")}, PunctVec{Punctuator::PlusPlus}),
                std::make_tuple(String{C("--")}, PunctVec{Punctuator::MinusMinus}),
                std::make_tuple(String{C("&")}, PunctVec{Punctuator::Ampersand}),
                std::make_tuple(String{C("*")}, PunctVec{Punctuator::Asterisk}),
                std::make_tuple(String{C("+")}, PunctVec{Punctuator::Plus}),
                std::make_tuple(String{C("-")}, PunctVec{Punctuator::Minus}),
                std::make_tuple(String{C("~")}, PunctVec{Punctuator::Tilde}),
                std::make_tuple(String{C("!")}, PunctVec{Punctuator::ExclamationMark}),
                std::make_tuple(String{C("/")}, PunctVec{Punctuator::Slash}),
                std::make_tuple(String{C("%")}, PunctVec{Punctuator::Percent}),
                std::make_tuple(String{C("<<")}, PunctVec{Punctuator::LessThanLessThan}),
                std::make_tuple(String{C(">>")}, PunctVec{Punctuator::GreaterThanGreaterThan}),
                std::make_tuple(String{C("<")}, PunctVec{Punctuator::LessThan}),
                std::make_tuple(String{C(">")}, PunctVec{Punctuator::GreaterThan}),
                std::make_tuple(String{C("<=")}, PunctVec{Punctuator::LessThanEqual}),
                std::make_tuple(String{C(">=")}, PunctVec{Punctuator::GreaterThanEqual}),
                std::make_tuple(String{C("==")}, PunctVec{Punctuator::EqualEqual}),
                std::make_tuple(String{C("!=")}, PunctVec{Punctuator::ExclamationMarkEqual}),
                std::make_tuple(String{C("^")}, PunctVec{Punctuator::Caret}),
                std::make_tuple(String{C("|")}, PunctVec{Punctuator::VerticalBar}),
                std::make_tuple(String{C("&&")}, PunctVec{Punctuator::AmpersandAmpersand}),
                std::make_tuple(String{C("||")}, PunctVec{Punctuator::VerticalBarVerticalBar}),
                std::make_tuple(String{C("?")}, PunctVec{Punctuator::QuestionMark}),
                std::make_tuple(String{C(":")}, PunctVec{Punctuator::Colon}),
                std::make_tuple(String{C(";")}, PunctVec{Punctuator::Semicolon}),
                std::make_tuple(String{C("...")}, PunctVec{Punctuator::Ellipsis}),
                std::make_tuple(String{C("=")}, PunctVec{Punctuator::Equal}),
                std::make_tuple(String{C("+=")}, PunctVec{Punctuator::PlusEqual}),
                std::make_tuple(String{C("-=")}, PunctVec{Punctuator::MinusEqual}),
                std::make_tuple(String{C("*=")}, PunctVec{Punctuator::AsteriskEqual}),
                std::make_tuple(String{C("/=")}, PunctVec{Punctuator::SlashEqual}),
                std::make_tuple(String{C("%=")}, PunctVec{Punctuator::PercentEqual}),
                std::make_tuple(String{C("<<=")}, PunctVec{Punctuator::LessThanLessThanEqual}),
                std::make_tuple(String{C(">>=")}, PunctVec{Punctuator::GreaterThanGreaterThanEqual}),
                std::make_tuple(String{C("&=")}, PunctVec{Punctuator::AmpersandEqual}),
                std::make_tuple(String{C("^=")}, PunctVec{Punctuator::CaretEqual}),
                std::make_tuple(String{C("|=")}, PunctVec{Punctuator::VerticalBarEqual}),
                std::make_tuple(String{C(",")}, PunctVec{Punctuator::Comma}),
                std::make_tuple(String{C("#")}, PunctVec{Punctuator::Hash}),
                std::make_tuple(String{C("##")}, PunctVec{Punctuator::HashHash}),
                std::make_tuple(String{C("<:")}, PunctVec{Punctuator::LeftBracket}),
                std::make_tuple(String{C(":>")}, PunctVec{Punctuator::RightBracket}),
                std::make_tuple(String{C("<%")}, PunctVec{Punctuator::LeftBrace}),
                std::make_tuple(String{C("%>")}, PunctVec{Punctuator::RightBrace}),
                std::make_tuple(String{C("%:")}, PunctVec{Punctuator::Hash}),
                std::make_tuple(String{C("%:%:")}, PunctVec{Punctuator::HashHash}),
                std::make_tuple(
                        String{C("|| &&")}, PunctVec{Punctuator::VerticalBarVerticalBar, Punctuator::AmpersandAmpersand}
                ),
                std::make_tuple(String{C("%:%")}, PunctVec{Punctuator::Hash, Punctuator::Percent})
        )
);
