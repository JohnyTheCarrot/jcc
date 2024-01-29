#ifndef JCC_PREPROCESSOR_H
#define JCC_PREPROCESSOR_H

#include "CompilerDatatypes.h"
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
	std::string m_Buffer;

public:
	using StringConstIter = CompilerDataTypes::StringConstIter;

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
		CompilerDataTypes::String m_Name;

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

	using Token = std::variant<
	        HeaderName, Identifier, PpNumber, CharacterConstant, StringConstant, Punctuator, Keyword, Directive>;
	using TokenList = std::vector<Token>;

private:
	std::unordered_map<CompilerDataTypes::StringView, Keyword> m_Keywords{
	        {COMP_STRING("auto"), Keyword::Auto},
	        {COMP_STRING("break"), Keyword::Break},
	        {COMP_STRING("case"), Keyword::Case},
	        {COMP_STRING("char"), Keyword::Char},
	        {COMP_STRING("const"), Keyword::Const},
	        {COMP_STRING("continue"), Keyword::Continue},
	        {COMP_STRING("default"), Keyword::Default},
	        {COMP_STRING("do"), Keyword::Do},
	        {COMP_STRING("double"), Keyword::Double},
	        {COMP_STRING("else"), Keyword::Else},
	        {COMP_STRING("enum"), Keyword::Enum},
	        {COMP_STRING("extern"), Keyword::Extern},
	        {COMP_STRING("float"), Keyword::Float},
	        {COMP_STRING("for"), Keyword::For},
	        {COMP_STRING("goto"), Keyword::Goto},
	        {COMP_STRING("if"), Keyword::If},
	        {COMP_STRING("inline"), Keyword::Inline},
	        {COMP_STRING("int"), Keyword::Int},
	        {COMP_STRING("long"), Keyword::Long},
	        {COMP_STRING("register"), Keyword::Register},
	        {COMP_STRING("restrict"), Keyword::Restrict},
	        {COMP_STRING("return"), Keyword::Return},
	        {COMP_STRING("short"), Keyword::Short},
	        {COMP_STRING("signed"), Keyword::Signed},
	        {COMP_STRING("sizeof"), Keyword::Sizeof},
	        {COMP_STRING("static"), Keyword::Static},
	        {COMP_STRING("struct"), Keyword::Struct},
	        {COMP_STRING("switch"), Keyword::Switch},
	        {COMP_STRING("typedef"), Keyword::Typedef},
	        {COMP_STRING("union"), Keyword::Union},
	        {COMP_STRING("unsigned"), Keyword::Unsigned},
	        {COMP_STRING("void"), Keyword::Void},
	        {COMP_STRING("volatile"), Keyword::Volatile},
	        {COMP_STRING("while"), Keyword::While},
	        {COMP_STRING("_Alignas"), Keyword::Alignas},
	        {COMP_STRING("_Alignof"), Keyword::Alignof},
	        {COMP_STRING("_Atomic"), Keyword::Atomic},
	        {COMP_STRING("_Bool"), Keyword::Bool},
	        {COMP_STRING("_Complex"), Keyword::Complex},
	        {COMP_STRING("_Generic"), Keyword::Generic},
	        {COMP_STRING("_Imaginary"), Keyword::Imaginary},
	        {COMP_STRING("_Noreturn"), Keyword::Noreturn},
	        {COMP_STRING("_Static_assert"), Keyword::StaticAssert},
	        {COMP_STRING("_Thread_local"), Keyword::ThreadLocal},
	};

	std::unordered_map<CompilerDataTypes::StringView, Directive> m_Directives{
	        {COMP_STRING("include"), Directive::Include}, {COMP_STRING("define"), Directive::Define},
	        {COMP_STRING("undef"), Directive::Undef},     {COMP_STRING("line"), Directive::Line},
	        {COMP_STRING("error"), Directive::Error},     {COMP_STRING("pragma"), Directive::Pragma},
	        {COMP_STRING("if"), Directive::If},           {COMP_STRING("ifdef"), Directive::Ifdef},
	        {COMP_STRING("ifndef"), Directive::Ifndef},   {COMP_STRING("elif"), Directive::Elif},
	        {COMP_STRING("else"), Directive::Else},       {COMP_STRING("endif"), Directive::Endif}
	};

	[[nodiscard]]
	std::optional<Keyword> MatchKeyword(const CompilerDataTypes::StringView &view) const;

	[[nodiscard]]
	std::optional<Directive> MatchDirective(const CompilerDataTypes::StringView &view) const;

	enum class ValidEscapeBase {
		Octal,
		Hexadecimal,
	};

	static constexpr size_t NUM_DIGITS_OCTAL_ESCAPE{3};

	bool TokenizeNumericalEscapeSequence(
	        StringConstIter &current, CompilerDataTypes::String &literalContent, Diagnosis::Vec &diagnoses,
	        unsigned long valueLimit, uint32_t valueMask, ValidEscapeBase base
	);

	enum class ConstantType {
		Character,
		String,
	};

	bool TokenizeCharacterOrStringLiteral(
	        StringConstIter &current, ConstantPrefix prefix, TokenList &tokens, Diagnosis::Vec &diagnoses,
	        Preprocessor::ConstantType type
	);

	[[nodiscard]]
	TokenList Tokenize(Diagnosis::Vec &diagnoses);

public:
	explicit Preprocessor(const CompilerDataTypes::String &buffer)
	    : m_Buffer{buffer} {
	}

	explicit Preprocessor(CompilerDataTypes::String &&buffer)
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
