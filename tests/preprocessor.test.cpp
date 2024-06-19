#include "misc/preprocessor/preprocessor.h"
#include "misc/Tokenizer.h"

#include <gtest/gtest.h>

using namespace jcc;

using Keyword             = Tokenizer::Keyword;
using Punctuator          = Tokenizer::Punctuator;
using Directive           = Tokenizer::Directive;
using Identifier          = Tokenizer::Identifier;
using CharacterConstant   = Tokenizer::CharacterConstant;
using StringConstant      = Tokenizer::StringConstant;
using DiagnosisKindVec    = std::vector<Diagnosis::Kind>;
using ConstantPrefix      = Tokenizer::ConstantPrefix;
using SpecialPurposeToken = Tokenizer::SpecialPurpose;
using PpNumber            = Tokenizer::PpNumber;
using IncludeDirective    = Tokenizer::IncludeDirective;

using TokenList = std::vector<Tokenizer::Token::Value>;

class PreproTest : public testing::TestWithParam<std::tuple<std::string, TokenList>> {};

TEST_P(PreproTest, Preprocessing) {
	std::istringstream         iss{std::get<0>(GetParam())};
	Diagnosis::Vec             diagnoses{};
	preprocessor::Preprocessor preprocessor{"test", iss, diagnoses};

	TokenList tokens{};

	std::transform(preprocessor.Current(), preprocessor.EndOfFile(), std::back_inserter(tokens), [](auto tokenValue) {
		return tokenValue.m_Value;
	});

	TokenList const expectedTokens{std::get<1>(GetParam())};

	EXPECT_EQ(tokens, expectedTokens);
}

INSTANTIATE_TEST_SUITE_P(
        Define, PreproTest,
        testing::Values(
                std::make_tuple(
                        R"(
#define NAME 1
NAME
)",
                        TokenList{PpNumber{"1"}}
                ),
                std::make_tuple(
                        R"(
#define NAME NAME
NAME
)",
                        TokenList{Identifier{"NAME"}}
                ),
                std::make_tuple(
                        // if this test fails, it means that the macro expansion somehow included the comment which would exclude tokens after the macro invocation
                        R"(
#define NAME 2 // not part of macro
NAME * 3
)",
                        TokenList{PpNumber{"2"}, Punctuator::Asterisk, PpNumber{"3"}}
                ),
                std::make_tuple(
                        R"(
#define SOME_MACRO 1 + 2
SOME_MACRO
)",
                        TokenList{PpNumber{"1"}, Punctuator::Plus, PpNumber{"2"}}
                ),
                std::make_tuple(
                        R"(
#define SOME_MACRO 4 + 6
SOME_MACRO * 2
)",
                        TokenList{PpNumber{"4"}, Punctuator::Plus, PpNumber{"6"}, Punctuator::Asterisk, PpNumber{"2"}}
                ),
                std::make_tuple(
                        R"(
#define FN_MACRO(a) a
FN_MACRO(90)
)",
                        TokenList{PpNumber{"90"}}
                ),
                std::make_tuple(
                        R"(
#define FN_MACRO(a) a + 8
FN_MACRO(9)
)",
                        TokenList{PpNumber{"9"}, Punctuator::Plus, PpNumber{"8"}}
                ),
                std::make_tuple(
                        R"(
#define FN_MACRO(a) a
FN_MACRO(68) + FN_MACRO(12)
)",
                        TokenList{PpNumber{"68"}, Punctuator::Plus, PpNumber{"12"}}
                ),
                std::make_tuple(
                        R"(
#define FN_MACRO(a, b) a - b
FN_MACRO(86, 56)
)",
                        TokenList{PpNumber{"86"}, Punctuator::Minus, PpNumber{"56"}}
                ),
                std::make_tuple(
                        R"(
#define FN_MACRO(a, b) (a - b)
FN_MACRO(86, 56)
)",
                        TokenList{
                                Punctuator::LeftParenthesis, PpNumber{"86"}, Punctuator::Minus, PpNumber{"56"},
                                Punctuator::RightParenthesis
                        }
                ),
                std::make_tuple(
                        R"(
#define NESTED(a) (a + 1)
NESTED(NESTED(5))
)",
                        TokenList{
                                Punctuator::LeftParenthesis, Punctuator::LeftParenthesis, PpNumber{"5"},
                                Punctuator::Plus, PpNumber{"1"}, Punctuator::RightParenthesis, Punctuator::Plus,
                                PpNumber{"1"}, Punctuator::RightParenthesis
                        }
                ),
                std::make_tuple(
                        R"(
#define VA(a, ...) {a, __VA_ARGS__}
VA(0)
)",
                        TokenList{Punctuator::LeftBrace, PpNumber{"0"}, Punctuator::RightBrace}
                ),
                std::make_tuple(
                        R"(
#define VA(a, ...) {a, __VA_ARGS__}
VA(1, 2)
)",
                        TokenList{
                                Punctuator::LeftBrace, PpNumber{"1"}, Punctuator::Comma, PpNumber{"2"},
                                Punctuator::RightBrace
                        }
                ),
                std::make_tuple(
                        R"(
#define VA(a, ...) {a, __VA_ARGS__}
VA(3, 4, 5)
)",
                        TokenList{
                                Punctuator::LeftBrace, PpNumber{"3"}, Punctuator::Comma, PpNumber{"4"},
                                Punctuator::Comma, PpNumber{"5"}, Punctuator::RightBrace
                        }
                ),
                std::make_tuple(
                        // test that __VA_ARGS__ is local to the current function macro. I.e, if another macro is called within the function-like macro, the current __VA_ARGS__ should not expand in said other macro.
                        R"(
#define SNEAKY __VA_ARGS__
#define VA(a, ...) {a, SNEAKY, __VA_ARGS__}
VA(6, 7)
)",
                        TokenList{
                                Punctuator::LeftBrace, PpNumber{"6"}, Punctuator::Comma, Identifier{"__VA_ARGS__"},
                                Punctuator::Comma, PpNumber{"7"}, Punctuator::RightBrace
                        }
                ),
                std::make_tuple(
                        // check that if another varaidic macro is called, that the `__VA_ARGS__` used in that macro correctly expands to the current macro's args and not the one that called it
                        R"(
#define OTHER_VA(...) someFn(__VA_ARGS__)
#define VA(a, ...) {a, OTHER_VA(17, __VA_ARGS__)}
VA(9, 1)
)",
                        TokenList{
                                Punctuator::LeftBrace, PpNumber{"9"}, Punctuator::Comma, Identifier{"someFn"},
                                Punctuator::LeftParenthesis, PpNumber{"17"}, Punctuator::Comma, PpNumber{"1"},
                                Punctuator::RightParenthesis, Punctuator::RightBrace
                        }
                ),
                std::make_tuple(
                        // check that after that macro is expanded, we don't lose the `__VA_ARGS__` of the top level macro
                        R"(
#define OTHER_VA(...) someFn(__VA_ARGS__)
#define VA(a, ...) {a, OTHER_VA(17, __VA_ARGS__), __VA_ARGS__}
VA(9, 1)
)",
                        TokenList{
                                Punctuator::LeftBrace, PpNumber{"9"}, Punctuator::Comma, Identifier{"someFn"},
                                Punctuator::LeftParenthesis, PpNumber{"17"}, Punctuator::Comma, PpNumber{"1"},
                                Punctuator::RightParenthesis, Punctuator::Comma, PpNumber{"1"}, Punctuator::RightBrace
                        }
                )
        )
);
