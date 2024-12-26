#include "tokenizer/tokenizer.h"

#include <magic_enum/magic_enum.hpp>

#include <gtest/gtest.h>

#include "misc/Diagnosis.h"

using namespace jcc::tokenizer;

using SpanMarker           = jcc::SpanMarker;
using Diagnosis            = jcc::Diagnosis;
using TokenList            = std::optional<Token::Value>;
using DiagnosisKindVec     = std::vector<jcc::Diagnosis::Kind>;
using TokenListOrException = std::variant<TokenList, jcc::Diagnosis::Kind>;

using TestData =
        std::tuple<std::string, TokenListOrException, DiagnosisKindVec>;

class TokenizerTest : public testing::TestWithParam<TestData> {};

TEST_P(TokenizerTest, Tokenizing) {
    auto const expectedTokenOrException{std::get<1>(GetParam())};

    auto const tokenize{[] {
        std::istringstream iss{std::get<0>(GetParam())};
        Diagnosis::Vec     diagnoses{};
        Tokenizer          tokenizer{iss};

        return tokenizer.GetNextToken();
    }};

    if (std::holds_alternative<Diagnosis::Kind>(expectedTokenOrException)) {
        auto const exceptionDiagKind{
                std::get<Diagnosis::Kind>(expectedTokenOrException)
        };
        try {
            auto const token{tokenize()};
            FAIL() << "Expected jcc::FatalCompilerError with kind "
                   << magic_enum::enum_name(exceptionDiagKind)
                   << " but got no exception, got token instead: "
                   << testing::PrintToString(token.value().m_Value);
        } catch (jcc::FatalCompilerError const &ex) {
            EXPECT_EQ(exceptionDiagKind, ex.GetDiagnosis().GetKind());
        } catch (...) {
            FAIL() << "Expected jcc::FatalCompilerError but got a different "
                      "exception";
        }
        return;
    }

    auto const token{tokenize()};
    auto const expectedToken{std::get<TokenList>(expectedTokenOrException)};

    ASSERT_EQ(token.has_value(), expectedToken.has_value());

    if (!token.has_value())
        return;

    EXPECT_EQ(token.value().m_Value, expectedToken);
}

INSTANTIATE_TEST_SUITE_P(
        Whitespace, TokenizerTest,
        testing::Values(
                std::make_tuple("", std::nullopt, DiagnosisKindVec{}),
                std::make_tuple(" ", std::nullopt, DiagnosisKindVec{}),
                std::make_tuple("\t", std::nullopt, DiagnosisKindVec{}),
                std::make_tuple("\v", std::nullopt, DiagnosisKindVec{}),
                std::make_tuple("\f", std::nullopt, DiagnosisKindVec{})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Keywords, TokenizerTest,
        testing::Values(
                std::make_tuple("auto", Keyword::Auto, DiagnosisKindVec{}),
                std::make_tuple("break", Keyword::Break, DiagnosisKindVec{}),
                std::make_tuple("case", Keyword::Case, DiagnosisKindVec{}),
                std::make_tuple("char", Keyword::Char, DiagnosisKindVec{}),
                std::make_tuple("const", Keyword::Const, DiagnosisKindVec{}),
                std::make_tuple(
                        "continue", Keyword::Continue, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "default", Keyword::Default, DiagnosisKindVec{}
                ),
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
                std::make_tuple(
                        "register", Keyword::Register, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "restrict", Keyword::Restrict, DiagnosisKindVec{}
                ),
                std::make_tuple("return", Keyword::Return, DiagnosisKindVec{}),
                std::make_tuple("short", Keyword::Short, DiagnosisKindVec{}),
                std::make_tuple("signed", Keyword::Signed, DiagnosisKindVec{}),
                std::make_tuple("sizeof", Keyword::Sizeof, DiagnosisKindVec{}),
                std::make_tuple("static", Keyword::Static, DiagnosisKindVec{}),
                std::make_tuple("struct", Keyword::Struct, DiagnosisKindVec{}),
                std::make_tuple("switch", Keyword::Switch, DiagnosisKindVec{}),
                std::make_tuple(
                        "typedef", Keyword::Typedef, DiagnosisKindVec{}
                ),
                std::make_tuple("union", Keyword::Union, DiagnosisKindVec{}),
                std::make_tuple(
                        "unsigned", Keyword::Unsigned, DiagnosisKindVec{}
                ),
                std::make_tuple("void", Keyword::Void, DiagnosisKindVec{}),
                std::make_tuple(
                        "volatile", Keyword::Volatile, DiagnosisKindVec{}
                ),
                std::make_tuple("while", Keyword::While, DiagnosisKindVec{}),
                std::make_tuple(
                        "_Alignas", Keyword::Alignas, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "_Alignof", Keyword::Alignof, DiagnosisKindVec{}
                ),
                std::make_tuple("_Atomic", Keyword::Atomic, DiagnosisKindVec{}),
                std::make_tuple("_Bool", Keyword::Bool, DiagnosisKindVec{}),
                std::make_tuple(
                        "_Complex", Keyword::Complex, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "_Generic", Keyword::Generic, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "_Imaginary", Keyword::Imaginary, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "_Noreturn", Keyword::Noreturn, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "_Static_assert", Keyword::StaticAssert,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "_Thread_local", Keyword::ThreadLocal,
                        DiagnosisKindVec{}
                ),
                std::make_tuple("false", Keyword::False, DiagnosisKindVec{}),
                std::make_tuple("true", Keyword::True, DiagnosisKindVec{}),
                std::make_tuple("nullptr", Keyword::Nullptr, DiagnosisKindVec{})
        )
);

INSTANTIATE_TEST_SUITE_P(
        Identifiers, TokenizerTest,
        testing::Values(
                // Identifiers starting with a char or string constant prefix are still identifiers and should be tokenized as such
                std::make_tuple(
                        "under_score", Identifier{"under_score"},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "u8SomeIdent", Identifier{"u8SomeIdent"},
                        DiagnosisKindVec{}
                ),
                // same for identifiers starting with a keyword
                std::make_tuple(
                        "autoSomeIdent", Identifier{"autoSomeIdent"},
                        DiagnosisKindVec{}
                ),
                std::make_tuple("L", Identifier{"L"}, DiagnosisKindVec{}),
                std::make_tuple("u", Identifier{"u"}, DiagnosisKindVec{}),
                std::make_tuple("U", Identifier{"U"}, DiagnosisKindVec{}),
                std::make_tuple("La", Identifier{"La"}, DiagnosisKindVec{}),
                std::make_tuple("ua", Identifier{"ua"}, DiagnosisKindVec{}),
                std::make_tuple("Ua", Identifier{"Ua"}, DiagnosisKindVec{}),
                std::make_tuple(
                        R"(\u0D9E)", Identifier{"\u0D9E"}, DiagnosisKindVec{}
                ),
                // escaped newlines legally may appear mid-token, because of course.
                std::make_tuple(
                        "ident\\\nifier", Identifier{"identifier"},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "ident\\\r\nifier", Identifier{"identifier"},
                        DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        Punctuators, TokenizerTest,
        testing::Values(
                std::make_tuple(
                        "[", Punctuator::LeftBracket, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "]", Punctuator::RightBracket, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        " (", Punctuator::LeftParenthesis, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "(", Punctuator::PpLeftParenthesis, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        ")", Punctuator::RightParenthesis, DiagnosisKindVec{}
                ),
                std::make_tuple("{", Punctuator::LeftBrace, DiagnosisKindVec{}),
                std::make_tuple(
                        "}", Punctuator::RightBrace, DiagnosisKindVec{}
                ),
                std::make_tuple(".", Punctuator::Dot, DiagnosisKindVec{}),
                std::make_tuple("->", Punctuator::Arrow, DiagnosisKindVec{}),
                std::make_tuple("++", Punctuator::PlusPlus, DiagnosisKindVec{}),
                std::make_tuple(
                        "--", Punctuator::MinusMinus, DiagnosisKindVec{}
                ),
                std::make_tuple("&", Punctuator::Ampersand, DiagnosisKindVec{}),
                std::make_tuple("*", Punctuator::Asterisk, DiagnosisKindVec{}),
                std::make_tuple("+", Punctuator::Plus, DiagnosisKindVec{}),
                std::make_tuple("-", Punctuator::Minus, DiagnosisKindVec{}),
                std::make_tuple("~", Punctuator::Tilde, DiagnosisKindVec{}),
                std::make_tuple(
                        "!", Punctuator::ExclamationMark, DiagnosisKindVec{}
                ),
                std::make_tuple("/", Punctuator::Slash, DiagnosisKindVec{}),
                std::make_tuple("%", Punctuator::Percent, DiagnosisKindVec{}),
                std::make_tuple(
                        "<<", Punctuator::LessThanLessThan, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        ">>", Punctuator::GreaterThanGreaterThan,
                        DiagnosisKindVec{}
                ),
                std::make_tuple("<", Punctuator::LessThan, DiagnosisKindVec{}),
                std::make_tuple(
                        ">", Punctuator::GreaterThan, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "<=", Punctuator::LessThanEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        ">=", Punctuator::GreaterThanEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "==", Punctuator::EqualEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "!=", Punctuator::ExclamationMarkEqual,
                        DiagnosisKindVec{}
                ),
                std::make_tuple("^", Punctuator::Caret, DiagnosisKindVec{}),
                std::make_tuple(
                        "|", Punctuator::VerticalBar, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "&&", Punctuator::AmpersandAmpersand, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "||", Punctuator::VerticalBarVerticalBar,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "?", Punctuator::QuestionMark, DiagnosisKindVec{}
                ),
                std::make_tuple(":", Punctuator::Colon, DiagnosisKindVec{}),
                std::make_tuple(";", Punctuator::Semicolon, DiagnosisKindVec{}),
                std::make_tuple(
                        "...", Punctuator::Ellipsis, DiagnosisKindVec{}
                ),
                std::make_tuple("=", Punctuator::Equal, DiagnosisKindVec{}),
                std::make_tuple(
                        "+=", Punctuator::PlusEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "-=", Punctuator::MinusEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "*=", Punctuator::AsteriskEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "/=", Punctuator::SlashEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "%=", Punctuator::PercentEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "<<=", Punctuator::LessThanLessThanEqual,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        ">>=", Punctuator::GreaterThanGreaterThanEqual,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "&=", Punctuator::AmpersandEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "^=", Punctuator::CaretEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "|=", Punctuator::VerticalBarEqual, DiagnosisKindVec{}
                ),
                std::make_tuple(",", Punctuator::Comma, DiagnosisKindVec{}),
                std::make_tuple("#", Punctuator::Hash, DiagnosisKindVec{}),
                std::make_tuple("##", Punctuator::HashHash, DiagnosisKindVec{}),
                std::make_tuple(
                        "<:", Punctuator::LeftBracket, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        ":>", Punctuator::RightBracket, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "<%", Punctuator::LeftBrace, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "%>", Punctuator::RightBrace, DiagnosisKindVec{}
                ),
                std::make_tuple("%:", Punctuator::Hash, DiagnosisKindVec{}),
                std::make_tuple(
                        "%:%:", Punctuator::HashHash, DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        Directives, TokenizerTest,
        testing::Values(
                std::make_tuple(
                        "#define", Directive::Define, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "#include", Directive::Include,
                        DiagnosisKindVec{Diagnosis::Kind::TK_ExpectedHeaderName}
                ),
                std::make_tuple("#undef", Directive::Undef, DiagnosisKindVec{}),
                std::make_tuple("#ifdef", Directive::Ifdef, DiagnosisKindVec{}),
                std::make_tuple(
                        "#ifndef", Directive::Ifndef, DiagnosisKindVec{}
                ),
                std::make_tuple("#elif", Directive::Elif, DiagnosisKindVec{}),
                std::make_tuple("#endif", Directive::Endif, DiagnosisKindVec{}),
                std::make_tuple("#line", Directive::Line, DiagnosisKindVec{}),
                std::make_tuple("#error", Directive::Error, DiagnosisKindVec{}),
                std::make_tuple(
                        "#pragma", Directive::Pragma, DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        CharacterConstants, TokenizerTest,
        testing::Values(
                std::make_tuple(
                        "'a'", CharacterConstant{'a'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\0')", CharacterConstant{'\0'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\1')", CharacterConstant{'\1'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\2')", CharacterConstant{'\2'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\3')", CharacterConstant{'\3'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\4')", CharacterConstant{'\4'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\5')", CharacterConstant{'\5'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\6')", CharacterConstant{'\6'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\7')", CharacterConstant{'\7'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\10')", CharacterConstant{'\10'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\8')", Diagnosis::Kind::TK_UnknownEscapeSequence,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\x0')", CharacterConstant{'\0'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\x00')", CharacterConstant{'\0'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\x7B')", CharacterConstant{'\x7B'},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\1234')",
                        CharacterConstant{
                                ('\123' << 8) | '4', ConstantPrefix::None
                        },
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('h)", Diagnosis::Kind::TK_CharUnterminated,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\')", Diagnosis::Kind::TK_CharUnterminated,
                        DiagnosisKindVec{Diagnosis::Kind::TK_CharUnterminated}
                ),
                std::make_tuple(
                        R"('\'')", CharacterConstant{'\''}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\"')", CharacterConstant{'\"'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\t')", CharacterConstant{'\t'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\v')", CharacterConstant{'\v'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\a')", CharacterConstant{'\a'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\b')", CharacterConstant{'\b'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\r')", CharacterConstant{'\r'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\f')", CharacterConstant{'\f'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\n')", CharacterConstant{'\n'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\\')", CharacterConstant{'\\'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\?')", CharacterConstant{'\?'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "'¥'", CharacterConstant{0xC2A5},
                        DiagnosisKindVec{Diagnosis::Kind::TK_CharOutOfRange}
                ),
                std::make_tuple(
                        "''", Diagnosis::Kind::TK_CharNoValue,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\x')", Diagnosis::Kind::TK_CharHexNoDigits,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "'ab'", CharacterConstant{'a' << 8 | 'b'},
                        DiagnosisKindVec{Diagnosis::Kind::TK_CharOutOfRange}
                ),
                std::make_tuple(
                        "u'ab'", Diagnosis::Kind::TK_UTFCharMoreThanOneChar,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "u8'cd'", Diagnosis::Kind::TK_UTFCharMoreThanOneChar,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "U'ef'", Diagnosis::Kind::TK_UTFCharMoreThanOneChar,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "L'gh'",
                        CharacterConstant{'g' << 8 | 'h', ConstantPrefix::L},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "u'a'", CharacterConstant{'a', ConstantPrefix::u},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "u8'b'", CharacterConstant{'b', ConstantPrefix::u8},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "U'c'", CharacterConstant{'c', ConstantPrefix::U},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "L'd'", CharacterConstant{'d', ConstantPrefix::L},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\u0099')",
                        Diagnosis::Kind::TK_IllegalUniversalCharacterName,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\U00000099')", SpecialPurpose::Error,
                        DiagnosisKindVec{
                                Diagnosis::Kind::
                                        TK_IllegalUniversalCharacterName
                        }
                ),
                std::make_tuple(
                        R"('\u005A')", SpecialPurpose::Error,
                        DiagnosisKindVec{
                                Diagnosis::Kind::
                                        TK_IllegalUniversalCharacterName
                        }
                ),
                std::make_tuple(
                        R"('\u0024')", CharacterConstant{'$'},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\u0040')", CharacterConstant{'@'},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"('\u0060')", CharacterConstant{'`'},
                        DiagnosisKindVec{}
                ),
                // too few digits
                std::make_tuple(
                        R"('\u006')", SpecialPurpose::Error,
                        DiagnosisKindVec{
                                Diagnosis::Kind::
                                        TK_InvalidUniversalCharacterName
                        }
                ),
                std::make_tuple(
                        R"('\U0006')", SpecialPurpose::Error,
                        DiagnosisKindVec{
                                Diagnosis::Kind::
                                        TK_InvalidUniversalCharacterName
                        }
                ),
                std::make_tuple(
                        R"('\U000006')", SpecialPurpose::Error,
                        DiagnosisKindVec{
                                Diagnosis::Kind::
                                        TK_InvalidUniversalCharacterName
                        }
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        StringConstants, TokenizerTest,
        testing::Values(
                std::make_tuple(
                        R"("")", StringConstant{""}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"("a")", StringConstant{"a"}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"("ab")", StringConstant{"ab"}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"("abc")", StringConstant{"abc"}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"("hello world")", StringConstant{"hello world"},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"("hello\nworld")", StringConstant{"hello\nworld"},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"("hello\\nworld")", StringConstant{R"(hello\nworld)"},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"("hello world)", Diagnosis::Kind::TK_StrUnterminated,
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"(L"hello world")",
                        StringConstant{"hello world", ConstantPrefix::L},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"(u8"ไทย")", StringConstant{"ไทย", ConstantPrefix::u8},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"("\"")", StringConstant{"\""}, DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        EscapedNewlines, TokenizerTest,
        testing::Values(
                std::make_tuple(
                        "conti\\\nnue", Keyword::Continue, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "+\\\n+", Punctuator::PlusPlus, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "\"Why must you \\\nhurt me so?\"",
                        StringConstant{"Why must you hurt me so?"},
                        DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "'\\\na'", CharacterConstant{'a'}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"(invalidEs\cape)",
                        Diagnosis::Kind::TK_IllegalBackslash, DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        Comments, TokenizerTest,
        testing::Values(
                std::make_tuple("// float", std::nullopt, DiagnosisKindVec{}),
                std::make_tuple(
                        "/* int */ float", Keyword::Float, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        R"(/*
this is supposed to be skipped
*/ true
			)",
                        Keyword::True, DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        NewLine, TokenizerTest,
        testing::Values(
                std::make_tuple(
                        "\n", SpecialPurpose::NewLine, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "\r\n", SpecialPurpose::NewLine, DiagnosisKindVec{}
                )
        )
);

INSTANTIATE_TEST_SUITE_P(
        PpNumbers, TokenizerTest,
        testing::Values(
                std::make_tuple("0", PpNumber{"0"}, DiagnosisKindVec{}),
                std::make_tuple("1", PpNumber{"1"}, DiagnosisKindVec{}),
                std::make_tuple("10", PpNumber{"10"}, DiagnosisKindVec{}),
                std::make_tuple(
                        "10E+10", PpNumber{"10E+10"}, DiagnosisKindVec{}
                ),
                std::make_tuple(
                        "0xF00D", PpNumber{"0xF00D"}, DiagnosisKindVec{}
                ),
                std::make_tuple("10.0", PpNumber{"10.0"}, DiagnosisKindVec{}),
                std::make_tuple("10Ex", PpNumber{"10Ex"}, DiagnosisKindVec{}),
                std::make_tuple(".15", PpNumber{".15"}, DiagnosisKindVec{}),
                std::make_tuple("16.", PpNumber{"16."}, DiagnosisKindVec{}),
                std::make_tuple("18ull", PpNumber{"18ull"}, DiagnosisKindVec{})
        )
);

class SpanGenerationTest
    : public testing::TestWithParam<
              std::tuple<std::string, SpanMarker, SpanMarker>> {};

TEST_P(SpanGenerationTest, SpanGeneration) {
    std::istringstream iss{std::get<0>(GetParam())};
    Diagnosis::Vec     diagnoses{};
    Tokenizer          tokenizer{iss};

    auto const optionalToken{tokenizer.GetNextToken()};
    ASSERT_TRUE(optionalToken.has_value());
    auto const [token, span]{optionalToken.value()};

    auto const expectedSpanStart{std::get<1>(GetParam())};
    auto const expectedSpanEnd{std::get<2>(GetParam())};

    EXPECT_EQ(span.m_Start, expectedSpanStart);
    EXPECT_EQ(span.m_End, expectedSpanEnd);
}

INSTANTIATE_TEST_SUITE_P(
        SpanGeneration, SpanGenerationTest,
        testing::Values(
                std::make_tuple(
                        "int", SpanMarker{1, 1, 0}, SpanMarker{1, 3, 2}
                ),
                std::make_tuple("in\\\nt", SpanMarker{1, 1}, SpanMarker{2, 1}),
                std::make_tuple("+", SpanMarker{1, 1}, SpanMarker{1, 1}),
                std::make_tuple("\\\n+", SpanMarker{2, 1}, SpanMarker{2, 1}),
                std::make_tuple("+\\\n+", SpanMarker{1, 1}, SpanMarker{2, 1}),
                std::make_tuple(
                        R"("hello world")", SpanMarker{1, 1},
                        SpanMarker{1, sizeof(R"("hello world")") - 1}
                ),
                std::make_tuple(
                        "\"hello\\\nworld\"", SpanMarker{1, 1},
                        SpanMarker{2, sizeof(R"(world")") - 1}
                )
        )
);
