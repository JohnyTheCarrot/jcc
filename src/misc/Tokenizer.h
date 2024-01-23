//
// Created by Tuur Martens on 12/01/2024.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "Config.h"
#include "SpanOld.h"
#include "Trie.h"


#include <iterator>
#include <variant>
#include <vector>

enum class TokenType {
	Increment,
	Plus,
	AddAssign,
	Decrement,
	Minus,
	SubtractAssign,
	BitwiseNot,
	Asterisk,
	MultiplyAssign,
	Divide,
	DivideAssign,
	Backslash,
	Semicolon,
	LeftBrace,
	RightBrace,
	LeftBracket,
	RightBracket,
	LeftParenthesis,
	RightParenthesis,
	ExclusiveOr,
	ExclusiveOrAssign,
	Dot,
	Arrow,
	Comma,
	Hash,
	LessThan,
	LessThanOrEqual,
	GreaterThan,
	GreaterThanOrEqual,
	ShiftLeft,
	ShiftLeftAssign,
	ShiftRight,
	ShiftRightAssign,
	LogicalOr,
	BitwiseOr,
	BitwiseOrAssign,
	LogicalAnd,
	Ampersand,
	BitwiseAndAssign,
	NotEquals,
	LogicalNot,
	Assign,
	Equals,
	QuestionMark,
	Colon,
	Modulo,
	ModuloAssign,
	IntegerLiteral,
	FloatLiteral,
	DoubleLiteral,
	StringLiteral,
	Ellipsis,

	// Keywords
	KeywordAuto,
	KeywordBreak,
	KeywordCase,
	KeywordChar,
	KeywordConst,
	KeywordContinue,
	KeywordDefault,
	KeywordDo,
	KeywordDouble,
	KeywordElse,
	KeywordEnum,
	KeywordExtern,
	KeywordFloat,
	KeywordFor,
	KeywordGoto,
	KeywordIf,
	KeywordInline,
	KeywordInt,
	KeywordLong,
	KeywordRegister,
	KeywordRestrict,
	KeywordReturn,
	KeywordShort,
	KeywordSigned,
	KeywordSizeof,
	KeywordStatic,
	KeywordStruct,
	KeywordSwitch,
	KeywordTypedef,
	KeywordUnion,
	KeywordUnsigned,
	KeywordVoid,
	KeywordVolatile,
	KeywordWhile,
	KeywordAlignas,
	KeywordAlignof,
	KeywordAtomic,
	KeywordBool,
	KeywordComplex,
	KeywordGeneric,
	KeywordImaginary,
	KeywordNoreturn,
	KeywordStaticAssert,
	KeywordThreadLocal,

	Identifier
};

enum class IntegerLiteralType {
	Int,
	Long,
	LongLong,
};

using IntegerTokenValue = int64_t;
using FloatingTokenValue = double;

struct IntegerLiteralTokenValue {
	IntegerTokenValue value{};
	bool isUnsigned{};
	IntegerLiteralType type{};
};

struct FloatingPointLiteralTokenValue {
	FloatingTokenValue value;
	bool isDouble;
};

typedef std::variant<IntegerLiteralTokenValue, FloatingPointLiteralTokenValue, std::string> TokenValue;

struct Token final {
	Token() = default;

	TokenType _type{};

	SpanOld _span{};

	TokenValue _value{};
};

class Tokenizer final {
	TrieNode<C('!'), C('~'), Char, TokenType> m_tokenDefs{};
	std::vector<Token> m_tokens{};
	IStream &m_istream;
	std::vector<String> m_lines{};
	size_t m_currentLine{};

	void Tokenize() {
		std::optional<TokenType> tokenType;
		while ((tokenType = m_tokenDefs.Find(m_istream)).has_value()) {

			//			 m_tokens.emplace_back({
			//			 		tokenType,
			//					Span{m_currentLine}
			//			 })
		}
	}

public:
	explicit Tokenizer(std::basic_istream<Char> &is)
		: m_istream{is} {
#pragma region token initialisation
		m_tokenDefs.Insert(C("+"), TokenType::Plus);
		m_tokenDefs.Insert(C("+="), TokenType::AddAssign);
		m_tokenDefs.Insert(C("++"), TokenType::Increment);
		m_tokenDefs.Insert(C("-"), TokenType::Minus);
		m_tokenDefs.Insert(C("-="), TokenType::SubtractAssign);
		m_tokenDefs.Insert(C("--"), TokenType::Decrement);
		m_tokenDefs.Insert(C("~"), TokenType::BitwiseNot);
		m_tokenDefs.Insert(C("*"), TokenType::Asterisk);
		m_tokenDefs.Insert(C("*="), TokenType::MultiplyAssign);
		m_tokenDefs.Insert(C("/"), TokenType::Divide);
		m_tokenDefs.Insert(C("/="), TokenType::DivideAssign);
		m_tokenDefs.Insert(C("\\"), TokenType::Backslash);
		m_tokenDefs.Insert(C(";"), TokenType::Semicolon);
		m_tokenDefs.Insert(C("{"), TokenType::LeftBrace);
		m_tokenDefs.Insert(C("}"), TokenType::RightBrace);
		m_tokenDefs.Insert(C("["), TokenType::LeftBracket);
		m_tokenDefs.Insert(C("]"), TokenType::RightBracket);
		m_tokenDefs.Insert(C("#"), TokenType::Hash);
		m_tokenDefs.Insert(C("\?\?="), TokenType::Hash);
		m_tokenDefs.Insert(C("\?\?("), TokenType::LeftBracket);
		m_tokenDefs.Insert(C("\?\?/"), TokenType::Backslash);
		m_tokenDefs.Insert(C("\?\?)"), TokenType::RightBracket);
		m_tokenDefs.Insert(C("\?\?'"), TokenType::RightBracket);
		m_tokenDefs.Insert(C("\?\?!"), TokenType::BitwiseOr);
		m_tokenDefs.Insert(C("\?\?>"), TokenType::RightBrace);
		m_tokenDefs.Insert(C("\?\?<"), TokenType::LeftBrace);
		m_tokenDefs.Insert(C("\?\?-"), TokenType::BitwiseNot);
		m_tokenDefs.Insert(C("<:"), TokenType::LeftBracket);
		m_tokenDefs.Insert(C(":>"), TokenType::RightBracket);
		m_tokenDefs.Insert(C("<%"), TokenType::LeftBrace);
		m_tokenDefs.Insert(C("%>"), TokenType::RightBrace);
		m_tokenDefs.Insert(C("("), TokenType::LeftParenthesis);
		m_tokenDefs.Insert(C(")"), TokenType::RightParenthesis);
		m_tokenDefs.Insert(C("^"), TokenType::ExclusiveOr);
		m_tokenDefs.Insert(C("^="), TokenType::ExclusiveOrAssign);
		m_tokenDefs.Insert(C("."), TokenType::Dot);
		m_tokenDefs.Insert(C("->"), TokenType::Arrow);
		m_tokenDefs.Insert(C(","), TokenType::Comma);
		m_tokenDefs.Insert(C("<"), TokenType::LessThan);
		m_tokenDefs.Insert(C("<="), TokenType::LessThanOrEqual);
		m_tokenDefs.Insert(C("<<"), TokenType::ShiftLeft);
		m_tokenDefs.Insert(C("<<="), TokenType::ShiftLeftAssign);
		m_tokenDefs.Insert(C(">>"), TokenType::ShiftRight);
		m_tokenDefs.Insert(C(">>="), TokenType::ShiftRightAssign);
		m_tokenDefs.Insert(C(">"), TokenType::GreaterThan);
		m_tokenDefs.Insert(C(">="), TokenType::GreaterThanOrEqual);
		m_tokenDefs.Insert(C("|"), TokenType::BitwiseOr);
		m_tokenDefs.Insert(C("|="), TokenType::BitwiseOrAssign);
		m_tokenDefs.Insert(C("||"), TokenType::LogicalOr);
		m_tokenDefs.Insert(C("&"), TokenType::Ampersand);
		m_tokenDefs.Insert(C("&="), TokenType::BitwiseAndAssign);
		m_tokenDefs.Insert(C("&&"), TokenType::LogicalAnd);
		m_tokenDefs.Insert(C("!"), TokenType::LogicalNot);
		m_tokenDefs.Insert(C("!="), TokenType::NotEquals);
		m_tokenDefs.Insert(C("="), TokenType::Assign);
		m_tokenDefs.Insert(C("=="), TokenType::Equals);
		m_tokenDefs.Insert(C("?"), TokenType::QuestionMark);
		m_tokenDefs.Insert(C(":"), TokenType::Colon);
		m_tokenDefs.Insert(C("%"), TokenType::Modulo);
		m_tokenDefs.Insert(C("%="), TokenType::ModuloAssign);
		m_tokenDefs.Insert(C("..."), TokenType::Ellipsis);

		// Keywords
		m_tokenDefs.Insert(C("auto"), TokenType::KeywordAuto);
		m_tokenDefs.Insert(C("break"), TokenType::KeywordBreak);
		m_tokenDefs.Insert(C("case"), TokenType::KeywordCase);
		m_tokenDefs.Insert(C("char"), TokenType::KeywordChar);
		m_tokenDefs.Insert(C("const"), TokenType::KeywordConst);
		m_tokenDefs.Insert(C("continue"), TokenType::KeywordContinue);
		m_tokenDefs.Insert(C("default"), TokenType::KeywordDefault);
		m_tokenDefs.Insert(C("do"), TokenType::KeywordDo);
		m_tokenDefs.Insert(C("double"), TokenType::KeywordDouble);
		m_tokenDefs.Insert(C("else"), TokenType::KeywordElse);
		m_tokenDefs.Insert(C("enum"), TokenType::KeywordEnum);
		m_tokenDefs.Insert(C("extern"), TokenType::KeywordExtern);
		m_tokenDefs.Insert(C("float"), TokenType::KeywordFloat);
		m_tokenDefs.Insert(C("for"), TokenType::KeywordFor);
		m_tokenDefs.Insert(C("goto"), TokenType::KeywordGoto);
		m_tokenDefs.Insert(C("if"), TokenType::KeywordIf);
		m_tokenDefs.Insert(C("inline"), TokenType::KeywordInline);
		m_tokenDefs.Insert(C("int"), TokenType::KeywordInt);
		m_tokenDefs.Insert(C("long"), TokenType::KeywordLong);
		m_tokenDefs.Insert(C("register"), TokenType::KeywordRegister);
		m_tokenDefs.Insert(C("restrict"), TokenType::KeywordRestrict);
		m_tokenDefs.Insert(C("return"), TokenType::KeywordReturn);
		m_tokenDefs.Insert(C("short"), TokenType::KeywordShort);
		m_tokenDefs.Insert(C("signed"), TokenType::KeywordSigned);
		m_tokenDefs.Insert(C("sizeof"), TokenType::KeywordSizeof);
		m_tokenDefs.Insert(C("static"), TokenType::KeywordStatic);
		m_tokenDefs.Insert(C("struct"), TokenType::KeywordStruct);
		m_tokenDefs.Insert(C("switch"), TokenType::KeywordSwitch);
		m_tokenDefs.Insert(C("typedef"), TokenType::KeywordTypedef);
		m_tokenDefs.Insert(C("union"), TokenType::KeywordUnion);
		m_tokenDefs.Insert(C("unsigned"), TokenType::KeywordUnsigned);
		m_tokenDefs.Insert(C("void"), TokenType::KeywordVoid);
		m_tokenDefs.Insert(C("volatile"), TokenType::KeywordVolatile);
		m_tokenDefs.Insert(C("while"), TokenType::KeywordWhile);
		m_tokenDefs.Insert(C("_Alignas"), TokenType::KeywordAlignas);
		m_tokenDefs.Insert(C("_Alignof"), TokenType::KeywordAlignof);
		m_tokenDefs.Insert(C("_Atomic"), TokenType::KeywordAtomic);
		m_tokenDefs.Insert(C("_Bool"), TokenType::KeywordBool);
		m_tokenDefs.Insert(C("_Complex"), TokenType::KeywordComplex);
		m_tokenDefs.Insert(C("_Generic"), TokenType::KeywordGeneric);
		m_tokenDefs.Insert(C("_Imaginary"), TokenType::KeywordImaginary);
		m_tokenDefs.Insert(C("_Noreturn"), TokenType::KeywordNoreturn);
		m_tokenDefs.Insert(C("_Static_assert"), TokenType::KeywordStaticAssert);
		m_tokenDefs.Insert(C("_Thread_local"), TokenType::KeywordThreadLocal);

		String str{};
		while (std::getline(m_istream, str, C('\n'))) { m_lines.push_back(std::move(str)); }
#pragma endregion
	}
};


#endif//TOKENIZER_H
