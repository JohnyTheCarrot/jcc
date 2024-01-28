#ifndef JCC_PREPROCESSOR_H
#define JCC_PREPROCESSOR_H

#include "Config.h"
#include "reporting.h"
#include <gtest/gtest-printers.h>
#include <string>
#include <unordered_map>
#include <variant>

// Preprocessing tokens:
// header-name
// identifier
// pp-number
// character-constant
// string-literal
// punctuator
// each non-white-space character that cannot be one of the above

class Preprocessor final {
	String m_Buffer;

public:
	struct HeaderName final {
		String m_Name;

		bool operator==(const HeaderName &other) const {
			return m_Name == other.m_Name;
		}

		friend void PrintTo(const HeaderName &headerName, std::ostream *os) {
			*os << testing::PrintToString(headerName.m_Name);
		}
	};

	struct Identifier final {
		String m_Name;

		bool operator==(const Identifier &other) const {
			return m_Name == other.m_Name;
		}

		friend void PrintTo(const Identifier &identifier, std::ostream *os) {
			*os << testing::PrintToString(identifier.m_Name);
		}
	};

	struct PpNumber final {
		// A preprocessing number has neither a value nor a type at the time of preprocessing.
		// It acquires both after a successful conversion to a number token.
		String m_Number;

		bool operator==(const PpNumber &other) const {
			return m_Number == other.m_Number;
		}
	};

	struct CharacterConstant final {
		Char m_Character;

		bool operator==(const CharacterConstant &other) const {
			return m_Character == other.m_Character;
		}
	};

	struct StringLiteral final {
		String m_String;

		bool operator==(const StringLiteral &other) const {
			return m_String == other.m_String;
		}
	};

	enum class Punctuator {
		LeftBracket,
		RightBracket,
		LeftParenthesis,
		RightParenthesis,
		LeftBrace,
		RightBrace,
		Dot,
		Arrow,
		PlusPlus,
		MinusMinus,
		Ampersand,
		Asterisk,
		Plus,
		Minus,
		Tilde,
		ExclamationMark,
		Slash,
		Percent,
		LessThanLessThan,
		GreaterThanGreaterThan,
		LessThan,
		GreaterThan,
		LessThanEqual,
		GreaterThanEqual,
		EqualEqual,
		ExclamationMarkEqual,
		Caret,
		VerticalBar,
		AmpersandAmpersand,
		VerticalBarVerticalBar,
		QuestionMark,
		Colon,
		Semicolon,
		Ellipsis,
		Equal,
		PlusEqual,
		MinusEqual,
		AsteriskEqual,
		SlashEqual,
		PercentEqual,
		LessThanLessThanEqual,
		GreaterThanGreaterThanEqual,
		AmpersandEqual,
		CaretEqual,
		VerticalBarEqual,
		Comma,
		Hash,
		HashHash,
		LessThanColon,
		ColonGreaterThan,
		LessThanPercent,
		PercentGreaterThan,
		PercentColon,
		PercentColonPercentColon,

		// used to separate normal tokens from partial/special purpose ones, don't just move willy nilly
		None,

		// Partial
		PartialEllipsis,
		PartialLeftShift,
		PartialRightShift,
		PartialHashHashDigraph,
		PartialHashHashDigraphStage2,
		HashHashDigraphFailure,
		EllipsisFailure,
	};

	enum class Keyword {
		Auto,
		Break,
		Case,
		Char,
		Const,
		Continue,
		Default,
		Do,
		Double,
		Else,
		Enum,
		Extern,
		Float,
		For,
		Goto,
		If,
		Inline,
		Int,
		Long,
		Register,
		Restrict,
		Return,
		Short,
		Signed,
		Sizeof,
		Static,
		Struct,
		Switch,
		Typedef,
		Union,
		Unsigned,
		Void,
		Volatile,
		While,
		Alignas,
		Alignof,
		Atomic,
		Bool,
		Complex,
		Generic,
		Imaginary,
		Noreturn,
		StaticAssert,
		ThreadLocal,
	};

	enum class Directive { Include, Define, Undef, Line, Error, Pragma, If, Ifdef, Ifndef, Elif, Else, Endif };

	using Token = std::variant<
	        HeaderName, Identifier, PpNumber, CharacterConstant, StringLiteral, Punctuator, Keyword, Directive>;
	using TokenList = std::vector<Token>;

private:
	std::unordered_map<StringView, Keyword> m_Keywords{
	        {C("auto"), Keyword::Auto},
	        {C("break"), Keyword::Break},
	        {C("case"), Keyword::Case},
	        {C("char"), Keyword::Char},
	        {C("const"), Keyword::Const},
	        {C("continue"), Keyword::Continue},
	        {C("default"), Keyword::Default},
	        {C("do"), Keyword::Do},
	        {C("double"), Keyword::Double},
	        {C("else"), Keyword::Else},
	        {C("enum"), Keyword::Enum},
	        {C("extern"), Keyword::Extern},
	        {C("float"), Keyword::Float},
	        {C("for"), Keyword::For},
	        {C("goto"), Keyword::Goto},
	        {C("if"), Keyword::If},
	        {C("inline"), Keyword::Inline},
	        {C("int"), Keyword::Int},
	        {C("long"), Keyword::Long},
	        {C("register"), Keyword::Register},
	        {C("restrict"), Keyword::Restrict},
	        {C("return"), Keyword::Return},
	        {C("short"), Keyword::Short},
	        {C("signed"), Keyword::Signed},
	        {C("sizeof"), Keyword::Sizeof},
	        {C("static"), Keyword::Static},
	        {C("struct"), Keyword::Struct},
	        {C("switch"), Keyword::Switch},
	        {C("typedef"), Keyword::Typedef},
	        {C("union"), Keyword::Union},
	        {C("unsigned"), Keyword::Unsigned},
	        {C("void"), Keyword::Void},
	        {C("volatile"), Keyword::Volatile},
	        {C("while"), Keyword::While},
	        {C("_Alignas"), Keyword::Alignas},
	        {C("_Alignof"), Keyword::Alignof},
	        {C("_Atomic"), Keyword::Atomic},
	        {C("_Bool"), Keyword::Bool},
	        {C("_Complex"), Keyword::Complex},
	        {C("_Generic"), Keyword::Generic},
	        {C("_Imaginary"), Keyword::Imaginary},
	        {C("_Noreturn"), Keyword::Noreturn},
	        {C("_Static_assert"), Keyword::StaticAssert},
	        {C("_Thread_local"), Keyword::ThreadLocal},
	};

	std::unordered_map<StringView, Directive> m_Directives{
	        {C("include"), Directive::Include}, {C("define"), Directive::Define}, {C("undef"), Directive::Undef},
	        {C("line"), Directive::Line},       {C("error"), Directive::Error},   {C("pragma"), Directive::Pragma},
	        {C("if"), Directive::If},           {C("ifdef"), Directive::Ifdef},   {C("ifndef"), Directive::Ifndef},
	        {C("elif"), Directive::Elif},       {C("else"), Directive::Else},     {C("endif"), Directive::Endif}
	};

	[[nodiscard]]
	std::optional<Keyword> MatchKeyword(const StringView &view) const;

	[[nodiscard]]
	std::optional<Directive> MatchDirective(const StringView &view) const;

	[[nodiscard]]
	TokenList Tokenize(Diagnosis::Vec &diagnoses);

public:
	explicit Preprocessor(const String &buffer)
	    : m_Buffer{buffer} {
	}

	explicit Preprocessor(String &&buffer)
	    : m_Buffer{std::move(buffer)} {
	}

	Preprocessor(const Preprocessor &)            = delete;
	Preprocessor(Preprocessor &&)                 = delete;
	Preprocessor &operator=(const Preprocessor &) = delete;
	Preprocessor &operator=(Preprocessor &&)      = delete;

	[[nodiscard]]
	TokenList Process(Diagnosis::Vec &diagnoses);
};

std::ostream &operator<<(std::ostream &os, Preprocessor::Punctuator punctuator);

std::ostream &operator<<(std::ostream &os, Preprocessor::Keyword keyword);

std::ostream &operator<<(std::ostream &os, Preprocessor::Directive directive);

#endif//JCC_PREPROCESSOR_H
