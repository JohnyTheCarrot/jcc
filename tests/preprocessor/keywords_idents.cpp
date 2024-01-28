#include <gtest/gtest.h>
#include <misc/Config.h>
#include <misc/Preprocessor.h>

class KeywordAndIdentTest : public testing::TestWithParam<std::tuple<String, Preprocessor::TokenList>> {};

TEST_P(KeywordAndIdentTest, KeywordsAndIdents) {
	Preprocessor preprocessor{std::get<0>(GetParam())};

	Diagnosis::Vec diagnoses{};
	const Preprocessor::TokenList tokenList{preprocessor.Process(diagnoses)};
	const auto expectedTokens{std::get<1>(GetParam())};

	ASSERT_GE(tokenList.size(), 1);
	EXPECT_EQ(expectedTokens, tokenList);
}

using Keyword   = Preprocessor::Keyword;
using TokenList = Preprocessor::TokenList;

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_KeywordsIdents, KeywordAndIdentTest,
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
                        String{C("float variableName")},
                        TokenList{Keyword::Float, Preprocessor::Identifier{C("variableName")}}
                ),
                std::make_tuple(String{C("under_score")}, TokenList{Preprocessor::Identifier{C("under_score")}})
        )
);
