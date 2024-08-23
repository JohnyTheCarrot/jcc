#ifndef JCC_TOKEN_H
#define JCC_TOKEN_H

#include "misc/Span.h"

#include <misc/compiler_data_types.h>
#include <string>
#include <variant>

namespace jcc::tokenizer {
	struct IncludeDirective final {
		std::string m_Name{};

		enum class HeaderType { HChar, QChar, MacroName } m_HeaderType;

		[[nodiscard]]
		bool
		operator==(IncludeDirective const &other) const;

		friend void PrintTo(IncludeDirective const &headerName, std::ostream *os);
	};

	struct Identifier final {
		using IdentString = std::string;
		IdentString m_Name{};

		[[nodiscard]]
		static bool IsValidFirstChar(char c);

		[[nodiscard]]
		static bool IsValidChar(char c);

		[[nodiscard]]
		bool
		operator==(Identifier const &other) const;

		friend void PrintTo(Identifier const &identifier, std::ostream *os);
	};

	struct PpNumber final {
		// A preprocessing number has neither a value nor a type at the time of preprocessing.
		// It acquires both after a successful conversion to a number token.
		std::string m_Number{};

		[[nodiscard]]
		bool
		operator==(PpNumber const &other) const;

		friend void PrintTo(PpNumber const &ppNumber, std::ostream *os);
	};

	enum class ConstantPrefix { None, L, u, U, u8 };

	[[nodiscard]]
	std::pair<compiler_data_types::Char32::type, compiler_data_types::Char32::type>
	GetConstantPrefixRange(ConstantPrefix prefix);

	ConstantPrefix ToConstantPrefix(std::string_view prefix);

	struct CharacterConstant final {
		compiler_data_types::Char32::type m_Character;
		ConstantPrefix                    m_Prefix;

		[[nodiscard]]
		bool
		operator==(CharacterConstant const &other) const;

		friend void PrintTo(CharacterConstant const &characterConstant, std::ostream *os);

		[[nodiscard]]
		std::string ToString() const;
	};

	struct StringConstant final {
		std::string    m_String;
		ConstantPrefix m_Prefix;

		[[nodiscard]]
		bool
		operator==(StringConstant const &other) const;

		friend void PrintTo(StringConstant const &stringConstant, std::ostream *os);

		[[nodiscard]]
		std::string ToString() const;
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
		PpLeftParenthesis,

		// used to separate normal tokens from partial/special purpose ones, don't just move willy nilly
		None,
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

	enum class SpecialPurpose {
		EndOfFile,
		Error,
		NewLine,
		Comment,
	};

	struct Token {
		using Value = std::variant<
		        IncludeDirective, Identifier, PpNumber, CharacterConstant, StringConstant, Punctuator, Keyword,
		        Directive, SpecialPurpose>;

		Value m_Value;
		Span  m_Span;

		[[nodiscard]]
		bool IsTerminal() const;

		template<typename T>
		[[nodiscard]]
		bool Is(T const &value) const {
			return std::holds_alternative<T>(m_Value) && std::get<T>(m_Value) == value;
		}

		[[nodiscard]]
		bool
		operator==(Token const &other) const;
	};

	void PrintTo(Keyword keyword, std::ostream *os);

	void PrintTo(Punctuator punctuator, std::ostream *os);

	void PrintTo(Directive directive, std::ostream *os);

	void PrintTo(SpecialPurpose specialPurpose, std::ostream *os);

	void PrintTo(ConstantPrefix constantPrefix, std::ostream *os);
}// namespace jcc::tokenizer

#endif//JCC_TOKEN_H