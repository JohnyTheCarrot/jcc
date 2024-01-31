#include <gtest/gtest.h>
#include <misc/Preprocessor.h>

using Keyword           = Preprocessor::Keyword;
using Punctuator        = Preprocessor::Punctuator;
using Directive         = Preprocessor::Directive;
using Identifier        = Preprocessor::Identifier;
using CharacterConstant = Preprocessor::CharacterConstant;
using StringConstant    = Preprocessor::StringConstant;
using DiagnosisKindVec  = std::vector<Diagnosis::Kind>;
using ConstantPrefix    = Preprocessor::ConstantPrefix;
using TokenList         = Preprocessor::TokenList;

class PpTokenizingTest
    : public testing::TestWithParam<std::tuple<std::string, Preprocessor::TokenList, DiagnosisKindVec>> {};

TEST_P(PpTokenizingTest, PpTokenizing) {
	Preprocessor preprocessor{std::get<0>(GetParam())};

	Diagnosis::Vec                diagnoses{};
	const Preprocessor::TokenList tokenList{preprocessor.Process(diagnoses)};
	const auto                    expectedTokens{std::get<1>(GetParam())};
	const auto                    expectedDiagnoses{std::get<2>(GetParam())};
	DiagnosisKindVec              actualDiagnosisKinds{};
	std::transform(
	        diagnoses.begin(), diagnoses.end(), std::back_inserter(actualDiagnosisKinds),
	        [](const auto &diagnosisKind) { return diagnosisKind.m_Kind; }
	);

	EXPECT_EQ(expectedDiagnoses, actualDiagnosisKinds);
	EXPECT_EQ(expectedTokens, tokenList);
}

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_KeywordsIdents, PpTokenizingTest,
        testing::Values(
                std::make_tuple("", TokenList{}, DiagnosisKindVec{}),
                std::make_tuple("auto", TokenList{Keyword::Auto}, DiagnosisKindVec{}),
                std::make_tuple("break", TokenList{Keyword::Break}, DiagnosisKindVec{}),
                std::make_tuple("case", TokenList{Keyword::Case}, DiagnosisKindVec{}),
                std::make_tuple("char", TokenList{Keyword::Char}, DiagnosisKindVec{}),
                std::make_tuple("const", TokenList{Keyword::Const}, DiagnosisKindVec{}),
                std::make_tuple("continue", TokenList{Keyword::Continue}, DiagnosisKindVec{}),
                std::make_tuple("default", TokenList{Keyword::Default}, DiagnosisKindVec{}),
                std::make_tuple("do", TokenList{Keyword::Do}, DiagnosisKindVec{}),
                std::make_tuple("double", TokenList{Keyword::Double}, DiagnosisKindVec{}),
                std::make_tuple("else", TokenList{Keyword::Else}, DiagnosisKindVec{}),
                std::make_tuple("enum", TokenList{Keyword::Enum}, DiagnosisKindVec{}),
                std::make_tuple("extern", TokenList{Keyword::Extern}, DiagnosisKindVec{}),
                std::make_tuple("float", TokenList{Keyword::Float}, DiagnosisKindVec{}),
                std::make_tuple("for", TokenList{Keyword::For}, DiagnosisKindVec{}),
                std::make_tuple("goto", TokenList{Keyword::Goto}, DiagnosisKindVec{}),
                std::make_tuple("if", TokenList{Keyword::If}, DiagnosisKindVec{}),
                std::make_tuple("inline", TokenList{Keyword::Inline}, DiagnosisKindVec{}),
                std::make_tuple("int", TokenList{Keyword::Int}, DiagnosisKindVec{}),
                std::make_tuple("long", TokenList{Keyword::Long}, DiagnosisKindVec{}),
                std::make_tuple("register", TokenList{Keyword::Register}, DiagnosisKindVec{}),
                std::make_tuple("restrict", TokenList{Keyword::Restrict}, DiagnosisKindVec{}),
                std::make_tuple("return", TokenList{Keyword::Return}, DiagnosisKindVec{}),
                std::make_tuple("short", TokenList{Keyword::Short}, DiagnosisKindVec{}),
                std::make_tuple("signed", TokenList{Keyword::Signed}, DiagnosisKindVec{}),
                std::make_tuple("sizeof", TokenList{Keyword::Sizeof}, DiagnosisKindVec{}),
                std::make_tuple("static", TokenList{Keyword::Static}, DiagnosisKindVec{}),
                std::make_tuple("struct", TokenList{Keyword::Struct}, DiagnosisKindVec{}),
                std::make_tuple("switch", TokenList{Keyword::Switch}, DiagnosisKindVec{}),
                std::make_tuple("typedef", TokenList{Keyword::Typedef}, DiagnosisKindVec{}),
                std::make_tuple("union", TokenList{Keyword::Union}, DiagnosisKindVec{}),
                std::make_tuple("unsigned", TokenList{Keyword::Unsigned}, DiagnosisKindVec{}),
                std::make_tuple("void", TokenList{Keyword::Void}, DiagnosisKindVec{}),
                std::make_tuple("volatile", TokenList{Keyword::Volatile}, DiagnosisKindVec{}),
                std::make_tuple("while", TokenList{Keyword::While}, DiagnosisKindVec{}),
                std::make_tuple("_Alignas", TokenList{Keyword::Alignas}, DiagnosisKindVec{}),
                std::make_tuple("_Alignof", TokenList{Keyword::Alignof}, DiagnosisKindVec{}),
                std::make_tuple("_Atomic", TokenList{Keyword::Atomic}, DiagnosisKindVec{}),
                std::make_tuple("_Bool", TokenList{Keyword::Bool}, DiagnosisKindVec{}),
                std::make_tuple("_Complex", TokenList{Keyword::Complex}, DiagnosisKindVec{}),
                std::make_tuple("_Generic", TokenList{Keyword::Generic}, DiagnosisKindVec{}),
                std::make_tuple("_Imaginary", TokenList{Keyword::Imaginary}, DiagnosisKindVec{}),
                std::make_tuple("_Noreturn", TokenList{Keyword::Noreturn}, DiagnosisKindVec{}),
                std::make_tuple("_Static_assert", TokenList{Keyword::StaticAssert}, DiagnosisKindVec{}),
                std::make_tuple("_Thread_local", TokenList{Keyword::ThreadLocal}, DiagnosisKindVec{}),
                std::make_tuple("sizeof float", TokenList{Keyword::Sizeof, Keyword::Float}, DiagnosisKindVec{}),
                std::make_tuple(
                        "float variableName", TokenList{Keyword::Float, Identifier{"variableName"}}, DiagnosisKindVec{}
                ),
                std::make_tuple("under_score", TokenList{Identifier{"under_score"}}, DiagnosisKindVec{}),
                // Identifiers starting with a char or string constant prefix are still identifiers and should be tokenized as such
                std::make_tuple("u8SomeIdent", TokenList{Identifier{"u8SomeIdent"}}, DiagnosisKindVec{}),
                // same for identifiers starting with a keyword
                std::make_tuple("autoSomeIdent", TokenList{Identifier{"autoSomeIdent"}}, DiagnosisKindVec{})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_Punctuators, PpTokenizingTest,
        testing::Values(
                std::make_tuple("[", TokenList{Punctuator::LeftBracket}, DiagnosisKindVec{}),
                std::make_tuple("]", TokenList{Punctuator::RightBracket}, DiagnosisKindVec{}),
                std::make_tuple("(", TokenList{Punctuator::LeftParenthesis}, DiagnosisKindVec{}),
                std::make_tuple(")", TokenList{Punctuator::RightParenthesis}, DiagnosisKindVec{}),
                std::make_tuple("{", TokenList{Punctuator::LeftBrace}, DiagnosisKindVec{}),
                std::make_tuple("}", TokenList{Punctuator::RightBrace}, DiagnosisKindVec{}),
                std::make_tuple(".", TokenList{Punctuator::Dot}, DiagnosisKindVec{}),
                std::make_tuple("->", TokenList{Punctuator::Arrow}, DiagnosisKindVec{}),
                std::make_tuple("++", TokenList{Punctuator::PlusPlus}, DiagnosisKindVec{}),
                std::make_tuple("--", TokenList{Punctuator::MinusMinus}, DiagnosisKindVec{}),
                std::make_tuple("&", TokenList{Punctuator::Ampersand}, DiagnosisKindVec{}),
                std::make_tuple("*", TokenList{Punctuator::Asterisk}, DiagnosisKindVec{}),
                std::make_tuple("+", TokenList{Punctuator::Plus}, DiagnosisKindVec{}),
                std::make_tuple("-", TokenList{Punctuator::Minus}, DiagnosisKindVec{}),
                std::make_tuple("~", TokenList{Punctuator::Tilde}, DiagnosisKindVec{}),
                std::make_tuple("!", TokenList{Punctuator::ExclamationMark}, DiagnosisKindVec{}),
                std::make_tuple("/", TokenList{Punctuator::Slash}, DiagnosisKindVec{}),
                std::make_tuple("%", TokenList{Punctuator::Percent}, DiagnosisKindVec{}),
                std::make_tuple("<<", TokenList{Punctuator::LessThanLessThan}, DiagnosisKindVec{}),
                std::make_tuple(">>", TokenList{Punctuator::GreaterThanGreaterThan}, DiagnosisKindVec{}),
                std::make_tuple("<", TokenList{Punctuator::LessThan}, DiagnosisKindVec{}),
                std::make_tuple(">", TokenList{Punctuator::GreaterThan}, DiagnosisKindVec{}),
                std::make_tuple("<=", TokenList{Punctuator::LessThanEqual}, DiagnosisKindVec{}),
                std::make_tuple(">=", TokenList{Punctuator::GreaterThanEqual}, DiagnosisKindVec{}),
                std::make_tuple("==", TokenList{Punctuator::EqualEqual}, DiagnosisKindVec{}),
                std::make_tuple("!=", TokenList{Punctuator::ExclamationMarkEqual}, DiagnosisKindVec{}),
                std::make_tuple("^", TokenList{Punctuator::Caret}, DiagnosisKindVec{}),
                std::make_tuple("|", TokenList{Punctuator::VerticalBar}, DiagnosisKindVec{}),
                std::make_tuple("&&", TokenList{Punctuator::AmpersandAmpersand}, DiagnosisKindVec{}),
                std::make_tuple("||", TokenList{Punctuator::VerticalBarVerticalBar}, DiagnosisKindVec{}),
                std::make_tuple("?", TokenList{Punctuator::QuestionMark}, DiagnosisKindVec{}),
                std::make_tuple(":", TokenList{Punctuator::Colon}, DiagnosisKindVec{}),
                std::make_tuple(";", TokenList{Punctuator::Semicolon}, DiagnosisKindVec{}),
                std::make_tuple("...", TokenList{Punctuator::Ellipsis}, DiagnosisKindVec{}),
                std::make_tuple("=", TokenList{Punctuator::Equal}, DiagnosisKindVec{}),
                std::make_tuple("+=", TokenList{Punctuator::PlusEqual}, DiagnosisKindVec{}),
                std::make_tuple("-=", TokenList{Punctuator::MinusEqual}, DiagnosisKindVec{}),
                std::make_tuple("*=", TokenList{Punctuator::AsteriskEqual}, DiagnosisKindVec{}),
                std::make_tuple("/=", TokenList{Punctuator::SlashEqual}, DiagnosisKindVec{}),
                std::make_tuple("%=", TokenList{Punctuator::PercentEqual}, DiagnosisKindVec{}),
                std::make_tuple("<<=", TokenList{Punctuator::LessThanLessThanEqual}, DiagnosisKindVec{}),
                std::make_tuple(">>=", TokenList{Punctuator::GreaterThanGreaterThanEqual}, DiagnosisKindVec{}),
                std::make_tuple("&=", TokenList{Punctuator::AmpersandEqual}, DiagnosisKindVec{}),
                std::make_tuple("^=", TokenList{Punctuator::CaretEqual}, DiagnosisKindVec{}),
                std::make_tuple("|=", TokenList{Punctuator::VerticalBarEqual}, DiagnosisKindVec{}),
                std::make_tuple(",", TokenList{Punctuator::Comma}, DiagnosisKindVec{}),
                std::make_tuple("#", TokenList{Punctuator::Hash}, DiagnosisKindVec{}),
                std::make_tuple("##", TokenList{Punctuator::HashHash}, DiagnosisKindVec{}),
                std::make_tuple("<:", TokenList{Punctuator::LeftBracket}, DiagnosisKindVec{}),
                std::make_tuple(":>", TokenList{Punctuator::RightBracket}, DiagnosisKindVec{}),
                std::make_tuple("<%", TokenList{Punctuator::LeftBrace}, DiagnosisKindVec{}),
                std::make_tuple("%>", TokenList{Punctuator::RightBrace}, DiagnosisKindVec{}),
                std::make_tuple("%:", TokenList{Punctuator::Hash}, DiagnosisKindVec{}),
                std::make_tuple("%:%:", TokenList{Punctuator::HashHash}, DiagnosisKindVec{}),
                std::make_tuple(
                        "|| &&", TokenList{Punctuator::VerticalBarVerticalBar, Punctuator::AmpersandAmpersand},
                        DiagnosisKindVec{}
                ),
                std::make_tuple("%:%", TokenList{}, DiagnosisKindVec{Diagnosis::Kind::PP_PartialTokenEncountered}),
                std::make_tuple("-+", TokenList{Punctuator::Minus, Punctuator::Plus}, DiagnosisKindVec{}),
                std::make_tuple("->*", TokenList{Punctuator::Arrow, Punctuator::Asterisk}, DiagnosisKindVec{}),
                std::make_tuple("*...", TokenList{Punctuator::Asterisk, Punctuator::Ellipsis}, DiagnosisKindVec{}),
                std::make_tuple("<^", TokenList{Punctuator::LessThan, Punctuator::Caret}, DiagnosisKindVec{})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_Directives, PpTokenizingTest,
        testing::Values(
                std::make_tuple("#define", TokenList{Directive::Define}, DiagnosisKindVec{}),
                std::make_tuple("#include", TokenList{Directive::Include}, DiagnosisKindVec{}),
                std::make_tuple("#undef", TokenList{Directive::Undef}, DiagnosisKindVec{}),
                std::make_tuple("#ifdef", TokenList{Directive::Ifdef}, DiagnosisKindVec{}),
                std::make_tuple("#ifndef", TokenList{Directive::Ifndef}, DiagnosisKindVec{}),
                std::make_tuple("#if", TokenList{Directive::If}, DiagnosisKindVec{}),
                std::make_tuple("#elif", TokenList{Directive::Elif}, DiagnosisKindVec{}),
                std::make_tuple("#else", TokenList{Directive::Else}, DiagnosisKindVec{}),
                std::make_tuple("#endif", TokenList{Directive::Endif}, DiagnosisKindVec{}),
                std::make_tuple("#line", TokenList{Directive::Line}, DiagnosisKindVec{}),
                std::make_tuple("#error", TokenList{Directive::Error}, DiagnosisKindVec{}),
                std::make_tuple("#pragma", TokenList{Directive::Pragma}, DiagnosisKindVec{}),
                std::make_tuple("# define", TokenList{Directive::Define}, DiagnosisKindVec{}),
                std::make_tuple("#\ninclude", TokenList{Punctuator::Hash, Identifier{"include"}}, DiagnosisKindVec{})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_CharacterConstants, PpTokenizingTest,
        testing::Values(
                std::make_tuple("'a'", TokenList{CharacterConstant{'a'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\0')", TokenList{CharacterConstant{'\0'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\1')", TokenList{CharacterConstant{'\1'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\2')", TokenList{CharacterConstant{'\2'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\3')", TokenList{CharacterConstant{'\3'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\4')", TokenList{CharacterConstant{'\4'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\5')", TokenList{CharacterConstant{'\5'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\6')", TokenList{CharacterConstant{'\6'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\7')", TokenList{CharacterConstant{'\7'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\10')", TokenList{CharacterConstant{'\10'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\8')", TokenList{}, DiagnosisKindVec{Diagnosis::Kind::PP_UnknownEscapeSequence}),
                std::make_tuple(R"('\x0')", TokenList{CharacterConstant{'\0'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\x00')", TokenList{CharacterConstant{'\0'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\x7B')", TokenList{CharacterConstant{'\x7B'}}, DiagnosisKindVec{}),
                std::make_tuple(
                        R"(u'\1234')", TokenList{CharacterConstant{('\123' << 8) | '4', ConstantPrefix::u}},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(R"('h)", TokenList{}, DiagnosisKindVec{Diagnosis::Kind::PP_CharUnterminated}),
                std::make_tuple(R"('\')", TokenList{}, DiagnosisKindVec{Diagnosis::Kind::PP_CharUnterminated}),
                std::make_tuple(R"('\'')", TokenList{CharacterConstant{'\''}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\"')", TokenList{CharacterConstant{'\"'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\t')", TokenList{CharacterConstant{'\t'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\v')", TokenList{CharacterConstant{'\v'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\a')", TokenList{CharacterConstant{'\a'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\b')", TokenList{CharacterConstant{'\b'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\r')", TokenList{CharacterConstant{'\r'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\f')", TokenList{CharacterConstant{'\f'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\n')", TokenList{CharacterConstant{'\n'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\\')", TokenList{CharacterConstant{'\\'}}, DiagnosisKindVec{}),
                std::make_tuple(R"('\?')", TokenList{CharacterConstant{'\?'}}, DiagnosisKindVec{}),
                std::make_tuple(
                        "'¥'", TokenList{CharacterConstant{0xC2A5}},
                        DiagnosisKindVec{Diagnosis::Kind::PP_CharOutOfRange}
                ),
                std::make_tuple("''", TokenList{}, DiagnosisKindVec{Diagnosis::Kind::PP_CharNoValue}),
                std::make_tuple(R"('\x')", TokenList{}, DiagnosisKindVec{Diagnosis::Kind::PP_CharHexNoDigits}),
                std::make_tuple(
                        "'ab'", TokenList{CharacterConstant{'a' << 8 | 'b'}},
                        DiagnosisKindVec{Diagnosis::Kind::PP_CharOutOfRange}
                ),
                std::make_tuple(
                        "u'ab'",
                        TokenList{CharacterConstant{
                                ('a' << 8 | 'b') & CompilerDataTypeInfo::CHAR16_T::MASK, ConstantPrefix::u
                        }},
                        DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        Preprocessor_StringConstants, PpTokenizingTest,
        testing::Values(
                std::make_tuple(R"("")", TokenList{StringConstant{""}}, DiagnosisKindVec{}),
                std::make_tuple(R"("a")", TokenList{StringConstant{"a"}}, DiagnosisKindVec{}),
                std::make_tuple(R"("ab")", TokenList{StringConstant{"ab"}}, DiagnosisKindVec{}),
                std::make_tuple(R"("abc")", TokenList{StringConstant{"abc"}}, DiagnosisKindVec{}),
                std::make_tuple(R"("hello world")", TokenList{StringConstant{"hello world"}}, DiagnosisKindVec{}),
                std::make_tuple(R"("hello\nworld")", TokenList{StringConstant{"hello\nworld"}}, DiagnosisKindVec{}),
                std::make_tuple(R"("hello\\nworld")", TokenList{StringConstant{R"(hello\nworld)"}}, DiagnosisKindVec{}),
                std::make_tuple(R"("hello world)", TokenList{}, DiagnosisKindVec{Diagnosis::Kind::PP_StrUnterminated}),
                std::make_tuple(
                        R"(L"hello world")", TokenList{StringConstant{"hello world", ConstantPrefix::L}},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(R"(u8"ไทย")", TokenList{StringConstant{"ไทย", ConstantPrefix::u8}}, DiagnosisKindVec{}),
                std::make_tuple(R"("\"")", TokenList{StringConstant{"\""}}, DiagnosisKindVec{})
        )
);
