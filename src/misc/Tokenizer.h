#ifndef JCC_TOKENIZER_H
#define JCC_TOKENIZER_H

#include "CharStream.h"
#include "Diagnosis.h"
#include "Trie.h"
#include "compiler_data_types.h"
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

class Tokenizer final {
public:
	struct HeaderName final {
		CompilerDataTypes::String m_Name;

		bool operator==(const HeaderName &other) const {
			return m_Name == other.m_Name;
		}

		friend void PrintTo(const HeaderName &headerName, std::ostream *os) {
			*os << testing::PrintToString(headerName.m_Name);
		}
	};

	struct Identifier final {
		std::basic_string<char32_t> m_Name;

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
		CompilerDataTypes::String m_Number;

		bool operator==(const PpNumber &other) const {
			return m_Number == other.m_Number;
		}
	};

	enum class ConstantPrefix { None, L, u, U, u8 };

	struct CharacterConstant final {
		uint32_t       m_Character;
		ConstantPrefix m_Prefix;

		bool operator==(const CharacterConstant &other) const {
			return m_Character == other.m_Character && m_Prefix == other.m_Prefix;
		}

		friend void PrintTo(const CharacterConstant &characterConstant, std::ostream *os);
	};

	struct StringConstant final {
		CompilerDataTypes::String m_String;
		ConstantPrefix            m_Prefix;

		bool operator==(const StringConstant &other) const {
			return m_String == other.m_String && m_Prefix == other.m_Prefix;
		}

		friend void PrintTo(const StringConstant &stringConstant, std::ostream *os);
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

	enum class SpecialPurpose {
		EndOfFile,
		Error,
		NewLine,
	};

	using Token = std::variant<
	        HeaderName, Identifier, PpNumber, CharacterConstant, StringConstant, Punctuator, Keyword, Directive,
	        SpecialPurpose>;

private:
	// TODO: replace unordered_map with trie
	using KeywordTrie = TrieNode<'A', 'z', Keyword>;
	KeywordTrie m_KeywordTrie{
	        {"auto", Keyword::Auto},
	        {"break", Keyword::Break},
	        {"case", Keyword::Case},
	        {"char", Keyword::Char},
	        {"const", Keyword::Const},
	        {"continue", Keyword::Continue},
	        {"default", Keyword::Default},
	        {"do", Keyword::Do},
	        {"double", Keyword::Double},
	        {"else", Keyword::Else},
	        {"enum", Keyword::Enum},
	        {"extern", Keyword::Extern},
	        {"float", Keyword::Float},
	        {"for", Keyword::For},
	        {"goto", Keyword::Goto},
	        {"if", Keyword::If},
	        {"inline", Keyword::Inline},
	        {"int", Keyword::Int},
	        {"long", Keyword::Long},
	        {"register", Keyword::Register},
	        {"restrict", Keyword::Restrict},
	        {"return", Keyword::Return},
	        {"short", Keyword::Short},
	        {"signed", Keyword::Signed},
	        {"sizeof", Keyword::Sizeof},
	        {"static", Keyword::Static},
	        {"struct", Keyword::Struct},
	        {"switch", Keyword::Switch},
	        {"typedef", Keyword::Typedef},
	        {"union", Keyword::Union},
	        {"unsigned", Keyword::Unsigned},
	        {"void", Keyword::Void},
	        {"volatile", Keyword::Volatile},
	        {"while", Keyword::While},
	        {"_Alignas", Keyword::Alignas},
	        {"_Alignof", Keyword::Alignof},
	        {"_Atomic", Keyword::Atomic},
	        {"_Bool", Keyword::Bool},
	        {"_Complex", Keyword::Complex},
	        {"_Generic", Keyword::Generic},
	        {"_Imaginary", Keyword::Imaginary},
	        {"_Noreturn", Keyword::Noreturn},
	        {"_Static_assert", Keyword::StaticAssert},
	        {"_Thread_local", Keyword::ThreadLocal},
	};

	TrieNode<'a', 'z', Directive> m_DirectiveTrie{
	        {"include", Directive::Include}, {"define", Directive::Define}, {"undef", Directive::Undef},
	        {"line", Directive::Line},       {"error", Directive::Error},   {"pragma", Directive::Pragma},
	        {"if", Directive::If},           {"ifdef", Directive::Ifdef},   {"ifndef", Directive::Ifndef},
	        {"elif", Directive::Elif},       {"else", Directive::Else},     {"endif", Directive::Endif},
	};

	enum class ValidEscapeBase {
		Octal,
		Hexadecimal,
	};

	static constexpr size_t NUM_DIGITS_OCTAL_ESCAPE{3};

	std::optional<CompilerDataTypes::Char> TokenizeNumericalEscapeSequence(ValidEscapeBase base);

	enum class ConstantType {
		Character,
		String,
	};

	[[nodiscard]]
	std::optional<char> TokenizeEscapeSequence();

	[[nodiscard]]
	Tokenizer::Token TokenizeCharacterOrStringLiteral(ConstantPrefix prefix, Tokenizer::ConstantType type);

	CharStream      m_Current;
	Diagnosis::Vec &m_Diagnoses;

	[[nodiscard]]
	Tokenizer::Punctuator TokenizeDot();

	[[nodiscard]]
	Punctuator TokenizeDash();

	[[nodiscard]]
	Punctuator TokenizePlus();

	[[nodiscard]]
	Punctuator TokenizeAmpersand();

	[[nodiscard]]
	Punctuator TokenizeVerticalBar();

	[[nodiscard]]
	Punctuator TokenizeAsterisk();

	[[nodiscard]]
	Punctuator TokenizeLessThan();

	[[nodiscard]]
	Punctuator TokenizeDoubleLessThan();

	[[nodiscard]]
	Punctuator TokenizeGreaterThan();

	[[nodiscard]]
	Punctuator TokenizeDoubleGreaterThan();

	[[nodiscard]]
	Punctuator TokenizeEqual();

	[[nodiscard]]
	Punctuator TokenizeExclamationMark();

	[[nodiscard]]
	Tokenizer::Token TokenizePercent();

	[[nodiscard]]
	Tokenizer::Token TokenizeHashHashDigraph();

	[[nodiscard]]
	Punctuator TokenizeCaret();

	[[nodiscard]]
	Punctuator TokenizeSlash();

	[[nodiscard]]
	Punctuator TokenizeColon();

	[[nodiscard]]
	Tokenizer::Punctuator TokenizeHash();

	[[nodiscard]]
	Tokenizer::Token TokenizePunctuator();

	[[nodiscard]]
	std::optional<Tokenizer::Token> TokenizeDirective();

	enum class UniversalCharacterNameType {
		u,
		U,
	};

	[[nodiscard]]
	std::optional<int> TokenizeHexDigit();

	[[nodiscard]]
	std::optional<char32_t> TokenizeUniversalCharacterName(UniversalCharacterNameType type);

	[[nodiscard]]
	Token TokenizeIdentifierOrKeyword();

	[[nodiscard]]
	Tokenizer::Token Tokenize();

public:
	Tokenizer(std::istream &iStream, Diagnosis::Vec &diagnoses)
	    : m_Current{iStream}
	    , m_Diagnoses{diagnoses} {
	}

	Tokenizer(const Tokenizer &)            = delete;
	Tokenizer(Tokenizer &&)                 = delete;
	Tokenizer &operator=(const Tokenizer &) = delete;
	Tokenizer &operator=(Tokenizer &&)      = delete;

	Token operator()();
};

std::ostream &operator<<(std::ostream &os, Tokenizer::SpecialPurpose specialPurpose);

std::ostream &operator<<(std::ostream &os, Tokenizer::Punctuator punctuator);

std::ostream &operator<<(std::ostream &os, Tokenizer::Keyword keyword);

std::ostream &operator<<(std::ostream &os, Tokenizer::Directive directive);

#endif//JCC_TOKENIZER_H
