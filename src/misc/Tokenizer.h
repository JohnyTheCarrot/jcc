//
// Created by Tuur Martens on 12/01/2024.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "Span.h"
#include "Trie.h"
#include "config.h"


#include <algorithm>
#include <iterator>
#include <variant>
#include <vector>

enum class TokenType {
	Util_NewLine,
	Util_LineComment,

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

using IntegerTokenValue  = int64_t;
using FloatingTokenValue = double;

struct IntegerLiteralTokenValue {
	IntegerTokenValue  value{};
	bool               isUnsigned{};
	IntegerLiteralType type{};
};

struct FloatingPointLiteralTokenValue {
	FloatingTokenValue value;
	bool               isDouble;
};

typedef std::variant<IntegerLiteralTokenValue, FloatingPointLiteralTokenValue, std::string> TokenValue;

struct Token final {
	TokenType m_Type{};

	Span m_Span{};

	std::optional<TokenValue> m_Value{};
};

using TokenList = std::vector<Token>;

class Tokenizer final {
public:
	static constexpr std::size_t FIRST_CHAR_INDEX{1};
	static constexpr std::size_t FIRST_LINE_INDEX{1};

private:
	TrieNode<C('\n'), C('~'), Char, TokenType> m_TokenDefs{};
	IStream                                   &m_IStream;
	std::vector<String>                        m_Lines{};
	std::size_t                                m_LineNumber{FIRST_LINE_INDEX};

public:
	void Tokenize(TokenList &tokens);

	explicit Tokenizer(IStream &is)
	    : m_IStream{is} {
#pragma region token initialisation
		// TODO: should the difference between \r\n and \n be handled by the preprocessor?
		m_TokenDefs.Insert(C("\r\n"), TokenType::Util_NewLine);
		m_TokenDefs.Insert(C("\n"), TokenType::Util_NewLine);

		// FIXME: this isn't part of the tokenizer, it should be handled by the preprocessor
		m_TokenDefs.Insert(C("//"), TokenType::Util_LineComment);

		m_TokenDefs.Insert(C("+"), TokenType::Plus);
		m_TokenDefs.Insert(C("+="), TokenType::AddAssign);
		m_TokenDefs.Insert(C("++"), TokenType::Increment);
		m_TokenDefs.Insert(C("-"), TokenType::Minus);
		m_TokenDefs.Insert(C("-="), TokenType::SubtractAssign);
		m_TokenDefs.Insert(C("--"), TokenType::Decrement);
		m_TokenDefs.Insert(C("~"), TokenType::BitwiseNot);
		m_TokenDefs.Insert(C("*"), TokenType::Asterisk);
		m_TokenDefs.Insert(C("*="), TokenType::MultiplyAssign);
		m_TokenDefs.Insert(C("/"), TokenType::Divide);
		m_TokenDefs.Insert(C("/="), TokenType::DivideAssign);
		m_TokenDefs.Insert(C(";"), TokenType::Semicolon);
		m_TokenDefs.Insert(C("{"), TokenType::LeftBrace);
		m_TokenDefs.Insert(C("}"), TokenType::RightBrace);
		m_TokenDefs.Insert(C("["), TokenType::LeftBracket);
		m_TokenDefs.Insert(C("]"), TokenType::RightBracket);

		// TODO: investigate if this is even relevant to the tokenizer... isn't it only relevant to the preprocessor?
		m_TokenDefs.Insert(C("#"), TokenType::Hash);

		// FIXME: irrelevant to the tokenizer, should be handled by the preprocessor
		m_TokenDefs.Insert(C("\\"), TokenType::Backslash);

		// FIXME: trigraphs aren't part of the tokenizer, they should be handled by the preprocessor
		m_TokenDefs.Insert(C("\?\?="), TokenType::Hash);
		m_TokenDefs.Insert(C("\?\?("), TokenType::LeftBracket);
		m_TokenDefs.Insert(C("\?\?/"), TokenType::Backslash);
		m_TokenDefs.Insert(C("\?\?)"), TokenType::RightBracket);
		m_TokenDefs.Insert(C("\?\?'"), TokenType::RightBracket);
		m_TokenDefs.Insert(C("\?\?!"), TokenType::BitwiseOr);
		m_TokenDefs.Insert(C("\?\?>"), TokenType::RightBrace);
		m_TokenDefs.Insert(C("\?\?<"), TokenType::LeftBrace);
		m_TokenDefs.Insert(C("\?\?-"), TokenType::BitwiseNot);

		// FIXME: digraphs aren't part of the tokenizer, they should be handled by the preprocessor
		m_TokenDefs.Insert(C("<:"), TokenType::LeftBracket);
		m_TokenDefs.Insert(C(":>"), TokenType::RightBracket);
		m_TokenDefs.Insert(C("<%"), TokenType::LeftBrace);
		m_TokenDefs.Insert(C("%>"), TokenType::RightBrace);

		m_TokenDefs.Insert(C("("), TokenType::LeftParenthesis);
		m_TokenDefs.Insert(C(")"), TokenType::RightParenthesis);
		m_TokenDefs.Insert(C("^"), TokenType::ExclusiveOr);
		m_TokenDefs.Insert(C("^="), TokenType::ExclusiveOrAssign);
		m_TokenDefs.Insert(C("."), TokenType::Dot);
		m_TokenDefs.Insert(C("->"), TokenType::Arrow);
		m_TokenDefs.Insert(C(","), TokenType::Comma);
		m_TokenDefs.Insert(C("<"), TokenType::LessThan);
		m_TokenDefs.Insert(C("<="), TokenType::LessThanOrEqual);
		m_TokenDefs.Insert(C("<<"), TokenType::ShiftLeft);
		m_TokenDefs.Insert(C("<<="), TokenType::ShiftLeftAssign);
		m_TokenDefs.Insert(C(">>"), TokenType::ShiftRight);
		m_TokenDefs.Insert(C(">>="), TokenType::ShiftRightAssign);
		m_TokenDefs.Insert(C(">"), TokenType::GreaterThan);
		m_TokenDefs.Insert(C(">="), TokenType::GreaterThanOrEqual);
		m_TokenDefs.Insert(C("|"), TokenType::BitwiseOr);
		m_TokenDefs.Insert(C("|="), TokenType::BitwiseOrAssign);
		m_TokenDefs.Insert(C("||"), TokenType::LogicalOr);
		m_TokenDefs.Insert(C("&"), TokenType::Ampersand);
		m_TokenDefs.Insert(C("&="), TokenType::BitwiseAndAssign);
		m_TokenDefs.Insert(C("&&"), TokenType::LogicalAnd);
		m_TokenDefs.Insert(C("!"), TokenType::LogicalNot);
		m_TokenDefs.Insert(C("!="), TokenType::NotEquals);
		m_TokenDefs.Insert(C("="), TokenType::Assign);
		m_TokenDefs.Insert(C("=="), TokenType::Equals);
		m_TokenDefs.Insert(C("?"), TokenType::QuestionMark);
		m_TokenDefs.Insert(C(":"), TokenType::Colon);
		m_TokenDefs.Insert(C("%"), TokenType::Modulo);
		m_TokenDefs.Insert(C("%="), TokenType::ModuloAssign);
		m_TokenDefs.Insert(C("..."), TokenType::Ellipsis);

		// Keywords
		m_TokenDefs.Insert(C("auto"), TokenType::KeywordAuto);
		m_TokenDefs.Insert(C("break"), TokenType::KeywordBreak);
		m_TokenDefs.Insert(C("case"), TokenType::KeywordCase);
		m_TokenDefs.Insert(C("char"), TokenType::KeywordChar);
		m_TokenDefs.Insert(C("const"), TokenType::KeywordConst);
		m_TokenDefs.Insert(C("continue"), TokenType::KeywordContinue);
		m_TokenDefs.Insert(C("default"), TokenType::KeywordDefault);
		m_TokenDefs.Insert(C("do"), TokenType::KeywordDo);
		m_TokenDefs.Insert(C("double"), TokenType::KeywordDouble);
		m_TokenDefs.Insert(C("else"), TokenType::KeywordElse);
		m_TokenDefs.Insert(C("enum"), TokenType::KeywordEnum);
		m_TokenDefs.Insert(C("extern"), TokenType::KeywordExtern);
		m_TokenDefs.Insert(C("float"), TokenType::KeywordFloat);
		m_TokenDefs.Insert(C("for"), TokenType::KeywordFor);
		m_TokenDefs.Insert(C("goto"), TokenType::KeywordGoto);
		m_TokenDefs.Insert(C("if"), TokenType::KeywordIf);
		m_TokenDefs.Insert(C("inline"), TokenType::KeywordInline);
		m_TokenDefs.Insert(C("int"), TokenType::KeywordInt);
		m_TokenDefs.Insert(C("long"), TokenType::KeywordLong);
		m_TokenDefs.Insert(C("register"), TokenType::KeywordRegister);
		m_TokenDefs.Insert(C("restrict"), TokenType::KeywordRestrict);
		m_TokenDefs.Insert(C("return"), TokenType::KeywordReturn);
		m_TokenDefs.Insert(C("short"), TokenType::KeywordShort);
		m_TokenDefs.Insert(C("signed"), TokenType::KeywordSigned);
		m_TokenDefs.Insert(C("sizeof"), TokenType::KeywordSizeof);
		m_TokenDefs.Insert(C("static"), TokenType::KeywordStatic);
		m_TokenDefs.Insert(C("struct"), TokenType::KeywordStruct);
		m_TokenDefs.Insert(C("switch"), TokenType::KeywordSwitch);
		m_TokenDefs.Insert(C("typedef"), TokenType::KeywordTypedef);
		m_TokenDefs.Insert(C("union"), TokenType::KeywordUnion);
		m_TokenDefs.Insert(C("unsigned"), TokenType::KeywordUnsigned);
		m_TokenDefs.Insert(C("void"), TokenType::KeywordVoid);
		m_TokenDefs.Insert(C("volatile"), TokenType::KeywordVolatile);
		m_TokenDefs.Insert(C("while"), TokenType::KeywordWhile);
		m_TokenDefs.Insert(C("_Alignas"), TokenType::KeywordAlignas);
		m_TokenDefs.Insert(C("_Alignof"), TokenType::KeywordAlignof);
		m_TokenDefs.Insert(C("_Atomic"), TokenType::KeywordAtomic);
		m_TokenDefs.Insert(C("_Bool"), TokenType::KeywordBool);
		m_TokenDefs.Insert(C("_Complex"), TokenType::KeywordComplex);
		m_TokenDefs.Insert(C("_Generic"), TokenType::KeywordGeneric);
		m_TokenDefs.Insert(C("_Imaginary"), TokenType::KeywordImaginary);
		m_TokenDefs.Insert(C("_Noreturn"), TokenType::KeywordNoreturn);
		m_TokenDefs.Insert(C("_Static_assert"), TokenType::KeywordStaticAssert);
		m_TokenDefs.Insert(C("_Thread_local"), TokenType::KeywordThreadLocal);

		//String str{};
		//while (std::getline(m_IStream, str, C('\n'))) { m_Lines.push_back(std::move(str)); }
#pragma endregion
	}
};


#endif//TOKENIZER_H
