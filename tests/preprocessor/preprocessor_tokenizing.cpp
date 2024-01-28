#include <gtest/gtest.h>
#include <misc/Config.h>
#include <misc/Preprocessor.h>

class PpTokenizingTest : public testing::TestWithParam<std::tuple<String, Preprocessor::TokenList>> {};

TEST_P(PpTokenizingTest, PpTokenizing) {
	Preprocessor preprocessor{std::get<0>(GetParam())};

	Diagnosis::Vec diagnoses{};
	const Preprocessor::TokenList tokenList{preprocessor.Process(diagnoses)};
	const auto expectedTokens{std::get<1>(GetParam())};

	ASSERT_GE(tokenList.size(), 1);
	EXPECT_EQ(expectedTokens, tokenList);
}

using Keyword    = Preprocessor::Keyword;
using Punctuator = Preprocessor::Punctuator;
using Directive  = Preprocessor::Directive;
using Identifier = Preprocessor::Identifier;
using TokenList  = Preprocessor::TokenList;

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_KeywordsIdents, PpTokenizingTest,
        testing::Values(
                std::make_tuple(String{C("auto")}, TokenList{Keyword::Auto}),
                std::make_tuple(String{C("break")}, TokenList{Keyword::Break}),
                std::make_tuple(String{C("case")}, TokenList{Keyword::Case}),
                std::make_tuple(String{C("char")}, TokenList{Keyword::Char}),
                std::make_tuple(String{C("const")}, TokenList{Keyword::Const}),
                std::make_tuple(String{C("continue")}, TokenList{Keyword::Continue}),
                std::make_tuple(String{C("default")}, TokenList{Keyword::Default}),
                std::make_tuple(String{C("do")}, TokenList{Keyword::Do}),
                std::make_tuple(String{C("double")}, TokenList{Keyword::Double}),
                std::make_tuple(String{C("else")}, TokenList{Keyword::Else}),
                std::make_tuple(String{C("enum")}, TokenList{Keyword::Enum}),
                std::make_tuple(String{C("extern")}, TokenList{Keyword::Extern}),
                std::make_tuple(String{C("float")}, TokenList{Keyword::Float}),
                std::make_tuple(String{C("for")}, TokenList{Keyword::For}),
                std::make_tuple(String{C("goto")}, TokenList{Keyword::Goto}),
                std::make_tuple(String{C("if")}, TokenList{Keyword::If}),
                std::make_tuple(String{C("inline")}, TokenList{Keyword::Inline}),
                std::make_tuple(String{C("int")}, TokenList{Keyword::Int}),
                std::make_tuple(String{C("long")}, TokenList{Keyword::Long}),
                std::make_tuple(String{C("register")}, TokenList{Keyword::Register}),
                std::make_tuple(String{C("restrict")}, TokenList{Keyword::Restrict}),
                std::make_tuple(String{C("return")}, TokenList{Keyword::Return}),
                std::make_tuple(String{C("short")}, TokenList{Keyword::Short}),
                std::make_tuple(String{C("signed")}, TokenList{Keyword::Signed}),
                std::make_tuple(String{C("sizeof")}, TokenList{Keyword::Sizeof}),
                std::make_tuple(String{C("static")}, TokenList{Keyword::Static}),
                std::make_tuple(String{C("struct")}, TokenList{Keyword::Struct}),
                std::make_tuple(String{C("switch")}, TokenList{Keyword::Switch}),
                std::make_tuple(String{C("typedef")}, TokenList{Keyword::Typedef}),
                std::make_tuple(String{C("union")}, TokenList{Keyword::Union}),
                std::make_tuple(String{C("unsigned")}, TokenList{Keyword::Unsigned}),
                std::make_tuple(String{C("void")}, TokenList{Keyword::Void}),
                std::make_tuple(String{C("volatile")}, TokenList{Keyword::Volatile}),
                std::make_tuple(String{C("while")}, TokenList{Keyword::While}),
                std::make_tuple(String{C("_Alignas")}, TokenList{Keyword::Alignas}),
                std::make_tuple(String{C("_Alignof")}, TokenList{Keyword::Alignof}),
                std::make_tuple(String{C("_Atomic")}, TokenList{Keyword::Atomic}),
                std::make_tuple(String{C("_Bool")}, TokenList{Keyword::Bool}),
                std::make_tuple(String{C("_Complex")}, TokenList{Keyword::Complex}),
                std::make_tuple(String{C("_Generic")}, TokenList{Keyword::Generic}),
                std::make_tuple(String{C("_Imaginary")}, TokenList{Keyword::Imaginary}),
                std::make_tuple(String{C("_Noreturn")}, TokenList{Keyword::Noreturn}),
                std::make_tuple(String{C("_Static_assert")}, TokenList{Keyword::StaticAssert}),
                std::make_tuple(String{C("_Thread_local")}, TokenList{Keyword::ThreadLocal}),
                std::make_tuple(String{C("sizeof float")}, TokenList{Keyword::Sizeof, Keyword::Float}),
                std::make_tuple(
                        String{C("float variableName")}, TokenList{Keyword::Float, Identifier{C("variableName")}}
                ),
                std::make_tuple(String{C("under_score")}, TokenList{Identifier{C("under_score")}})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_Punctuators, PpTokenizingTest,
        testing::Values(
                std::make_tuple(String{C("[")}, TokenList{Punctuator::LeftBracket}),
                std::make_tuple(String{C("]")}, TokenList{Punctuator::RightBracket}),
                std::make_tuple(String{C("(")}, TokenList{Punctuator::LeftParenthesis}),
                std::make_tuple(String{C(")")}, TokenList{Punctuator::RightParenthesis}),
                std::make_tuple(String{C("{")}, TokenList{Punctuator::LeftBrace}),
                std::make_tuple(String{C("}")}, TokenList{Punctuator::RightBrace}),
                std::make_tuple(String{C(".")}, TokenList{Punctuator::Dot}),
                std::make_tuple(String{C("->")}, TokenList{Punctuator::Arrow}),
                std::make_tuple(String{C("++")}, TokenList{Punctuator::PlusPlus}),
                std::make_tuple(String{C("--")}, TokenList{Punctuator::MinusMinus}),
                std::make_tuple(String{C("&")}, TokenList{Punctuator::Ampersand}),
                std::make_tuple(String{C("*")}, TokenList{Punctuator::Asterisk}),
                std::make_tuple(String{C("+")}, TokenList{Punctuator::Plus}),
                std::make_tuple(String{C("-")}, TokenList{Punctuator::Minus}),
                std::make_tuple(String{C("~")}, TokenList{Punctuator::Tilde}),
                std::make_tuple(String{C("!")}, TokenList{Punctuator::ExclamationMark}),
                std::make_tuple(String{C("/")}, TokenList{Punctuator::Slash}),
                std::make_tuple(String{C("%")}, TokenList{Punctuator::Percent}),
                std::make_tuple(String{C("<<")}, TokenList{Punctuator::LessThanLessThan}),
                std::make_tuple(String{C(">>")}, TokenList{Punctuator::GreaterThanGreaterThan}),
                std::make_tuple(String{C("<")}, TokenList{Punctuator::LessThan}),
                std::make_tuple(String{C(">")}, TokenList{Punctuator::GreaterThan}),
                std::make_tuple(String{C("<=")}, TokenList{Punctuator::LessThanEqual}),
                std::make_tuple(String{C(">=")}, TokenList{Punctuator::GreaterThanEqual}),
                std::make_tuple(String{C("==")}, TokenList{Punctuator::EqualEqual}),
                std::make_tuple(String{C("!=")}, TokenList{Punctuator::ExclamationMarkEqual}),
                std::make_tuple(String{C("^")}, TokenList{Punctuator::Caret}),
                std::make_tuple(String{C("|")}, TokenList{Punctuator::VerticalBar}),
                std::make_tuple(String{C("&&")}, TokenList{Punctuator::AmpersandAmpersand}),
                std::make_tuple(String{C("||")}, TokenList{Punctuator::VerticalBarVerticalBar}),
                std::make_tuple(String{C("?")}, TokenList{Punctuator::QuestionMark}),
                std::make_tuple(String{C(":")}, TokenList{Punctuator::Colon}),
                std::make_tuple(String{C(";")}, TokenList{Punctuator::Semicolon}),
                std::make_tuple(String{C("...")}, TokenList{Punctuator::Ellipsis}),
                std::make_tuple(String{C("=")}, TokenList{Punctuator::Equal}),
                std::make_tuple(String{C("+=")}, TokenList{Punctuator::PlusEqual}),
                std::make_tuple(String{C("-=")}, TokenList{Punctuator::MinusEqual}),
                std::make_tuple(String{C("*=")}, TokenList{Punctuator::AsteriskEqual}),
                std::make_tuple(String{C("/=")}, TokenList{Punctuator::SlashEqual}),
                std::make_tuple(String{C("%=")}, TokenList{Punctuator::PercentEqual}),
                std::make_tuple(String{C("<<=")}, TokenList{Punctuator::LessThanLessThanEqual}),
                std::make_tuple(String{C(">>=")}, TokenList{Punctuator::GreaterThanGreaterThanEqual}),
                std::make_tuple(String{C("&=")}, TokenList{Punctuator::AmpersandEqual}),
                std::make_tuple(String{C("^=")}, TokenList{Punctuator::CaretEqual}),
                std::make_tuple(String{C("|=")}, TokenList{Punctuator::VerticalBarEqual}),
                std::make_tuple(String{C(",")}, TokenList{Punctuator::Comma}),
                std::make_tuple(String{C("#")}, TokenList{Punctuator::Hash}),
                std::make_tuple(String{C("##")}, TokenList{Punctuator::HashHash}),
                std::make_tuple(String{C("<:")}, TokenList{Punctuator::LeftBracket}),
                std::make_tuple(String{C(":>")}, TokenList{Punctuator::RightBracket}),
                std::make_tuple(String{C("<%")}, TokenList{Punctuator::LeftBrace}),
                std::make_tuple(String{C("%>")}, TokenList{Punctuator::RightBrace}),
                std::make_tuple(String{C("%:")}, TokenList{Punctuator::Hash}),
                std::make_tuple(String{C("%:%:")}, TokenList{Punctuator::HashHash}),
                std::make_tuple(
                        String{C("|| &&")},
                        TokenList{Punctuator::VerticalBarVerticalBar, Punctuator::AmpersandAmpersand}
                ),
                std::make_tuple(String{C("%:%")}, TokenList{Punctuator::Hash, Punctuator::Percent})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_Directives, PpTokenizingTest,
        testing::Values(
                std::make_tuple(String{C("#define")}, TokenList{Directive::Define}),
                std::make_tuple(String{C("#include")}, TokenList{Directive::Include}),
                std::make_tuple(String{C("#undef")}, TokenList{Directive::Undef}),
                std::make_tuple(String{C("#ifdef")}, TokenList{Directive::Ifdef}),
                std::make_tuple(String{C("#ifndef")}, TokenList{Directive::Ifndef}),
                std::make_tuple(String{C("#if")}, TokenList{Directive::If}),
                std::make_tuple(String{C("#elif")}, TokenList{Directive::Elif}),
                std::make_tuple(String{C("#else")}, TokenList{Directive::Else}),
                std::make_tuple(String{C("#endif")}, TokenList{Directive::Endif}),
                std::make_tuple(String{C("#line")}, TokenList{Directive::Line}),
                std::make_tuple(String{C("#error")}, TokenList{Directive::Error}),
                std::make_tuple(String{C("#pragma")}, TokenList{Directive::Pragma}),
                std::make_tuple(String{C("# define")}, TokenList{Directive::Define}),
                std::make_tuple(String{C("#\ninclude")}, TokenList{Punctuator::Hash, Identifier{C("include")}})
        )
);
