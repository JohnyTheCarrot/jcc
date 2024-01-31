#include <gtest/gtest.h>
#include <misc/Preprocessor.h>

using Keyword             = Preprocessor::Keyword;
using Punctuator          = Preprocessor::Punctuator;
using Directive           = Preprocessor::Directive;
using Identifier          = Preprocessor::Identifier;
using CharacterConstant   = Preprocessor::CharacterConstant;
using StringConstant      = Preprocessor::StringConstant;
using DiagnosisKindVec    = std::vector<Diagnosis::Kind>;
using ConstantPrefix      = Preprocessor::ConstantPrefix;
using SpecialPurposeToken = Preprocessor::SpecialPurpose;
using TokenList           = Preprocessor::TokenList;

class PpTokenizingTest
    : public testing::TestWithParam<std::tuple<std::string, Preprocessor::Token, DiagnosisKindVec>> {};

TEST_P(PpTokenizingTest, PpTokenizing) {
	std::istringstream iss{std::get<0>(GetParam())};
	Diagnosis::Vec     diagnoses{};
	Preprocessor       preprocessor{std::move(iss), diagnoses};

	const Preprocessor::Token token{preprocessor()};

	const Preprocessor::Token expectedToken{std::get<1>(GetParam())};
	const auto                expectedDiagnoses{std::get<2>(GetParam())};
	DiagnosisKindVec          actualDiagnosisKinds{};
	std::transform(
	        diagnoses.begin(), diagnoses.end(), std::back_inserter(actualDiagnosisKinds),
	        [](const auto &diagnosisKind) { return diagnosisKind.m_Kind; }
	);

	EXPECT_EQ(expectedDiagnoses, actualDiagnosisKinds);
	EXPECT_EQ(expectedToken, token);
}

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_KeywordsIdents, PpTokenizingTest,
        testing::Values(
                std::make_tuple("", SpecialPurposeToken::EndOfFile, DiagnosisKindVec{}),
                std::make_tuple("auto", Keyword::Auto, DiagnosisKindVec{}),
                std::make_tuple("break", Keyword::Break, DiagnosisKindVec{}),
                std::make_tuple("case", Keyword::Case, DiagnosisKindVec{}),
                std::make_tuple("char", Keyword::Char, DiagnosisKindVec{}),
                std::make_tuple("const", Keyword::Const, DiagnosisKindVec{}),
                std::make_tuple("continue", Keyword::Continue, DiagnosisKindVec{}),
                std::make_tuple("default", Keyword::Default, DiagnosisKindVec{}),
                std::make_tuple("do", Keyword::Do, DiagnosisKindVec{}),
                std::make_tuple("double", Keyword::Double, DiagnosisKindVec{}),
                std::make_tuple("else", Keyword::Else, DiagnosisKindVec{}),
                std::make_tuple("enum", Keyword::Enum, DiagnosisKindVec{}),
                std::make_tuple("extern", Keyword::Extern, DiagnosisKindVec{}),
                std::make_tuple("float", Keyword::Float, DiagnosisKindVec{}),
                std::make_tuple("for", Keyword::For, DiagnosisKindVec{}),
                std::make_tuple("goto", Keyword::Goto, DiagnosisKindVec{}),
                std::make_tuple("if", Keyword::If, DiagnosisKindVec{}),
                std::make_tuple("inline", Keyword::Inline, DiagnosisKindVec{}),
                std::make_tuple("int", Keyword::Int, DiagnosisKindVec{}),
                std::make_tuple("long", Keyword::Long, DiagnosisKindVec{}),
                std::make_tuple("register", Keyword::Register, DiagnosisKindVec{}),
                std::make_tuple("restrict", Keyword::Restrict, DiagnosisKindVec{}),
                std::make_tuple("return", Keyword::Return, DiagnosisKindVec{}),
                std::make_tuple("short", Keyword::Short, DiagnosisKindVec{}),
                std::make_tuple("signed", Keyword::Signed, DiagnosisKindVec{}),
                std::make_tuple("sizeof", Keyword::Sizeof, DiagnosisKindVec{}),
                std::make_tuple("static", Keyword::Static, DiagnosisKindVec{}),
                std::make_tuple("struct", Keyword::Struct, DiagnosisKindVec{}),
                std::make_tuple("switch", Keyword::Switch, DiagnosisKindVec{}),
                std::make_tuple("typedef", Keyword::Typedef, DiagnosisKindVec{}),
                std::make_tuple("union", Keyword::Union, DiagnosisKindVec{}),
                std::make_tuple("unsigned", Keyword::Unsigned, DiagnosisKindVec{}),
                std::make_tuple("void", Keyword::Void, DiagnosisKindVec{}),
                std::make_tuple("volatile", Keyword::Volatile, DiagnosisKindVec{}),
                std::make_tuple("while", Keyword::While, DiagnosisKindVec{}),
                std::make_tuple("_Alignas", Keyword::Alignas, DiagnosisKindVec{}),
                std::make_tuple("_Alignof", Keyword::Alignof, DiagnosisKindVec{}),
                std::make_tuple("_Atomic", Keyword::Atomic, DiagnosisKindVec{}),
                std::make_tuple("_Bool", Keyword::Bool, DiagnosisKindVec{}),
                std::make_tuple("_Complex", Keyword::Complex, DiagnosisKindVec{}),
                std::make_tuple("_Generic", Keyword::Generic, DiagnosisKindVec{}),
                std::make_tuple("_Imaginary", Keyword::Imaginary, DiagnosisKindVec{}),
                std::make_tuple("_Noreturn", Keyword::Noreturn, DiagnosisKindVec{}),
                std::make_tuple("_Static_assert", Keyword::StaticAssert, DiagnosisKindVec{}),
                std::make_tuple("_Thread_local", Keyword::ThreadLocal, DiagnosisKindVec{}),
                std::make_tuple("under_score", Identifier{"under_score"}, DiagnosisKindVec{}),
                // Identifiers starting with a char or string constant prefix are still identifiers and should be tokenized as such
                std::make_tuple("u8SomeIdent", Identifier{"u8SomeIdent"}, DiagnosisKindVec{}),
                // same for identifiers starting with a keyword
                std::make_tuple("autoSomeIdent", Identifier{"autoSomeIdent"}, DiagnosisKindVec{})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_Punctuators, PpTokenizingTest,
        testing::Values(
                std::make_tuple("[", Punctuator::LeftBracket, DiagnosisKindVec{}),
                std::make_tuple("]", Punctuator::RightBracket, DiagnosisKindVec{}),
                std::make_tuple("(", Punctuator::LeftParenthesis, DiagnosisKindVec{}),
                std::make_tuple(")", Punctuator::RightParenthesis, DiagnosisKindVec{}),
                std::make_tuple("{", Punctuator::LeftBrace, DiagnosisKindVec{}),
                std::make_tuple("}", Punctuator::RightBrace, DiagnosisKindVec{}),
                std::make_tuple(".", Punctuator::Dot, DiagnosisKindVec{}),
                std::make_tuple("->", Punctuator::Arrow, DiagnosisKindVec{}),
                std::make_tuple("++", Punctuator::PlusPlus, DiagnosisKindVec{}),
                std::make_tuple("--", Punctuator::MinusMinus, DiagnosisKindVec{}),
                std::make_tuple("&", Punctuator::Ampersand, DiagnosisKindVec{}),
                std::make_tuple("*", Punctuator::Asterisk, DiagnosisKindVec{}),
                std::make_tuple("+", Punctuator::Plus, DiagnosisKindVec{}),
                std::make_tuple("-", Punctuator::Minus, DiagnosisKindVec{}),
                std::make_tuple("~", Punctuator::Tilde, DiagnosisKindVec{}),
                std::make_tuple("!", Punctuator::ExclamationMark, DiagnosisKindVec{}),
                std::make_tuple("/", Punctuator::Slash, DiagnosisKindVec{}),
                std::make_tuple("%", Punctuator::Percent, DiagnosisKindVec{}),
                std::make_tuple("<<", Punctuator::LessThanLessThan, DiagnosisKindVec{}),
                std::make_tuple(">>", Punctuator::GreaterThanGreaterThan, DiagnosisKindVec{}),
                std::make_tuple("<", Punctuator::LessThan, DiagnosisKindVec{}),
                std::make_tuple(">", Punctuator::GreaterThan, DiagnosisKindVec{}),
                std::make_tuple("<=", Punctuator::LessThanEqual, DiagnosisKindVec{}),
                std::make_tuple(">=", Punctuator::GreaterThanEqual, DiagnosisKindVec{}),
                std::make_tuple("==", Punctuator::EqualEqual, DiagnosisKindVec{}),
                std::make_tuple("!=", Punctuator::ExclamationMarkEqual, DiagnosisKindVec{}),
                std::make_tuple("^", Punctuator::Caret, DiagnosisKindVec{}),
                std::make_tuple("|", Punctuator::VerticalBar, DiagnosisKindVec{}),
                std::make_tuple("&&", Punctuator::AmpersandAmpersand, DiagnosisKindVec{}),
                std::make_tuple("||", Punctuator::VerticalBarVerticalBar, DiagnosisKindVec{}),
                std::make_tuple("?", Punctuator::QuestionMark, DiagnosisKindVec{}),
                std::make_tuple(":", Punctuator::Colon, DiagnosisKindVec{}),
                std::make_tuple(";", Punctuator::Semicolon, DiagnosisKindVec{}),
                std::make_tuple("...", Punctuator::Ellipsis, DiagnosisKindVec{}),
                std::make_tuple("=", Punctuator::Equal, DiagnosisKindVec{}),
                std::make_tuple("+=", Punctuator::PlusEqual, DiagnosisKindVec{}),
                std::make_tuple("-=", Punctuator::MinusEqual, DiagnosisKindVec{}),
                std::make_tuple("*=", Punctuator::AsteriskEqual, DiagnosisKindVec{}),
                std::make_tuple("/=", Punctuator::SlashEqual, DiagnosisKindVec{}),
                std::make_tuple("%=", Punctuator::PercentEqual, DiagnosisKindVec{}),
                std::make_tuple("<<=", Punctuator::LessThanLessThanEqual, DiagnosisKindVec{}),
                std::make_tuple(">>=", Punctuator::GreaterThanGreaterThanEqual, DiagnosisKindVec{}),
                std::make_tuple("&=", Punctuator::AmpersandEqual, DiagnosisKindVec{}),
                std::make_tuple("^=", Punctuator::CaretEqual, DiagnosisKindVec{}),
                std::make_tuple("|=", Punctuator::VerticalBarEqual, DiagnosisKindVec{}),
                std::make_tuple(",", Punctuator::Comma, DiagnosisKindVec{}),
                std::make_tuple("#", Punctuator::Hash, DiagnosisKindVec{}),
                std::make_tuple("##", Punctuator::HashHash, DiagnosisKindVec{}),
                std::make_tuple("<:", Punctuator::LeftBracket, DiagnosisKindVec{}),
                std::make_tuple(":>", Punctuator::RightBracket, DiagnosisKindVec{}),
                std::make_tuple("<%", Punctuator::LeftBrace, DiagnosisKindVec{}),
                std::make_tuple("%>", Punctuator::RightBrace, DiagnosisKindVec{}),
                std::make_tuple("%:", Punctuator::Hash, DiagnosisKindVec{}),
                std::make_tuple("%:%:", Punctuator::HashHash, DiagnosisKindVec{}),
                std::make_tuple(
                        "%:%", SpecialPurposeToken::Error, DiagnosisKindVec{Diagnosis::Kind::PP_PartialTokenEncountered}
                )
        )
);
//
INSTANTIATE_TEST_SUITE_P(
        Preprocessor_Directives, PpTokenizingTest,
        testing::Values(
                std::make_tuple("#define", Directive::Define, DiagnosisKindVec{}),
                std::make_tuple("#include", Directive::Include, DiagnosisKindVec{}),
                std::make_tuple("#undef", Directive::Undef, DiagnosisKindVec{}),
                std::make_tuple("#ifdef", Directive::Ifdef, DiagnosisKindVec{}),
                std::make_tuple("#ifndef", Directive::Ifndef, DiagnosisKindVec{}),
                std::make_tuple("#if", Directive::If, DiagnosisKindVec{}),
                std::make_tuple("#elif", Directive::Elif, DiagnosisKindVec{}),
                std::make_tuple("#else", Directive::Else, DiagnosisKindVec{}),
                std::make_tuple("#endif", Directive::Endif, DiagnosisKindVec{}),
                std::make_tuple("#line", Directive::Line, DiagnosisKindVec{}),
                std::make_tuple("#error", Directive::Error, DiagnosisKindVec{}),
                std::make_tuple("#pragma", Directive::Pragma, DiagnosisKindVec{}),
                std::make_tuple("# define", Directive::Define, DiagnosisKindVec{})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_CharacterConstants, PpTokenizingTest,
        testing::Values(
                std::make_tuple("'a'", CharacterConstant{'a'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\0')", CharacterConstant{'\0'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\1')", CharacterConstant{'\1'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\2')", CharacterConstant{'\2'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\3')", CharacterConstant{'\3'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\4')", CharacterConstant{'\4'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\5')", CharacterConstant{'\5'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\6')", CharacterConstant{'\6'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\7')", CharacterConstant{'\7'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\10')", CharacterConstant{'\10'}, DiagnosisKindVec{}),
                std::make_tuple(
                        R"('\8')", SpecialPurposeToken::Error,
                        DiagnosisKindVec{Diagnosis::Kind::PP_UnknownEscapeSequence}
                ),
                std::make_tuple(R"('\x0')", CharacterConstant{'\0'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\x00')", CharacterConstant{'\0'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\x7B')", CharacterConstant{'\x7B'}, DiagnosisKindVec{}),
                std::make_tuple(
                        R"(u'\1234')", CharacterConstant{('\123' << 8) | '4', ConstantPrefix::u}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('h)", SpecialPurposeToken::Error, DiagnosisKindVec{Diagnosis::Kind::PP_CharUnterminated}
                ),
                std::make_tuple(
                        R"('\')", SpecialPurposeToken::Error, DiagnosisKindVec{Diagnosis::Kind::PP_CharUnterminated}
                ),
                std::make_tuple(R"('\'')", CharacterConstant{'\''}, DiagnosisKindVec{}),
                std::make_tuple(R"('\"')", CharacterConstant{'\"'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\t')", CharacterConstant{'\t'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\v')", CharacterConstant{'\v'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\a')", CharacterConstant{'\a'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\b')", CharacterConstant{'\b'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\r')", CharacterConstant{'\r'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\f')", CharacterConstant{'\f'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\n')", CharacterConstant{'\n'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\\')", CharacterConstant{'\\'}, DiagnosisKindVec{}),
                std::make_tuple(R"('\?')", CharacterConstant{'\?'}, DiagnosisKindVec{}),
                std::make_tuple("'¥'", CharacterConstant{0xC2A5}, DiagnosisKindVec{Diagnosis::Kind::PP_CharOutOfRange}),
                std::make_tuple("''", SpecialPurposeToken::Error, DiagnosisKindVec{Diagnosis::Kind::PP_CharNoValue}),
                std::make_tuple(
                        R"('\x')", SpecialPurposeToken::Error, DiagnosisKindVec{Diagnosis::Kind::PP_CharHexNoDigits}
                ),
                std::make_tuple(
                        "'ab'", CharacterConstant{'a' << 8 | 'b'}, DiagnosisKindVec{Diagnosis::Kind::PP_CharOutOfRange}
                ),
                std::make_tuple(
                        "u'ab'",
                        CharacterConstant{('a' << 8 | 'b') & CompilerDataTypeInfo::CHAR16_T::MASK, ConstantPrefix::u},
                        DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_StringConstants, PpTokenizingTest,
        testing::Values(
                std::make_tuple(R"("")", StringConstant{""}, DiagnosisKindVec{}),
                std::make_tuple(R"("a")", StringConstant{"a"}, DiagnosisKindVec{}),
                std::make_tuple(R"("ab")", StringConstant{"ab"}, DiagnosisKindVec{}),
                std::make_tuple(R"("abc")", StringConstant{"abc"}, DiagnosisKindVec{}),
                std::make_tuple(R"("hello world")", StringConstant{"hello world"}, DiagnosisKindVec{}),
                std::make_tuple(R"("hello\nworld")", StringConstant{"hello\nworld"}, DiagnosisKindVec{}),
                std::make_tuple(R"("hello\\nworld")", StringConstant{R"(hello\nworld)"}, DiagnosisKindVec{}),
                std::make_tuple(
                        R"("hello world)", SpecialPurposeToken::Error,
                        DiagnosisKindVec{Diagnosis::Kind::PP_StrUnterminated}
                ),
                std::make_tuple(
                        R"(L"hello world")", StringConstant{"hello world", ConstantPrefix::L}, DiagnosisKindVec{}
                ),
                std::make_tuple(R"(u8"ไทย")", StringConstant{"ไทย", ConstantPrefix::u8}, DiagnosisKindVec{}),
                std::make_tuple(R"("\"")", StringConstant{"\""}, DiagnosisKindVec{})
        )
);
