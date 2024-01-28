#ifndef JCC_PREPROCESSOR_H
#define JCC_PREPROCESSOR_H

#include "Config.h"
#include "reporting.h"
#include <string>
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
	};

	struct Identifier final {
		String m_Name;
	};

	struct PpNumber final {
		// A preprocessing number has neither a value nor a type at the time of preprocessing.
		// It acquires both after a successful conversion to a number token.
		String m_Number;
	};

	struct CharacterConstant final {
		Char m_Character;
	};

	struct StringLiteral final {
		String m_String;
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

	using Token = std::variant<HeaderName, Identifier, PpNumber, CharacterConstant, StringLiteral, Punctuator, Keyword>;
	using TokenList = std::vector<Token>;

private:
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

#endif//JCC_PREPROCESSOR_H
