#ifndef JCC_TOKENIZER_H
#define JCC_TOKENIZER_H

#include "CharStream.h"
#include "Diagnosis.h"
#include "Trie.h"
#include "compiler_data_types.h"
#include <gtest/gtest-printers.h>
#include <magic_enum/magic_enum.hpp>
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

namespace jcc {
	class Tokenizer final {
	public:
		struct IncludeDirective final {
			std::string m_Name{};

			enum class HeaderType { HChar, QChar, MacroName } m_HeaderType;

			bool operator==(IncludeDirective const &other) const {
				return m_Name == other.m_Name;
			}

			friend void PrintTo(IncludeDirective const &headerName, std::ostream *os) {
				*os << "IncludeDirective(" << magic_enum::enum_name(headerName.m_HeaderType) << ", "
				    << testing::PrintToString(headerName.m_Name) << ')';
			}
		};

		struct Identifier final {
			using IdentString = std::string;
			IdentString m_Name{};

			bool operator==(Identifier const &other) const {
				return m_Name == other.m_Name;
			}

			friend void PrintTo(Identifier const &identifier, std::ostream *os) {
				*os << "Ident(" << testing::PrintToString(identifier.m_Name) << ')';
			}

			[[nodiscard]]
			std::string ToString() const;
		};

		struct PpNumber final {
			// A preprocessing number has neither a value nor a type at the time of preprocessing.
			// It acquires both after a successful conversion to a number token.
			std::string m_Number{};

			bool operator==(PpNumber const &other) const {
				return m_Number == other.m_Number;
			}

			friend void PrintTo(PpNumber const &ppNumber, std::ostream *os) {
				*os << "PpNumber(" << ppNumber.m_Number << ')';
			}

			[[nodiscard]]
			std::string ToString() const;
		};

		enum class ConstantPrefix { None, L, u, U, u8 };

		struct CharacterConstant final {
			uint32_t       m_Character;
			ConstantPrefix m_Prefix;

			bool operator==(CharacterConstant const &other) const {
				return m_Character == other.m_Character && m_Prefix == other.m_Prefix;
			}

			friend void PrintTo(CharacterConstant const &characterConstant, std::ostream *os);

			[[nodiscard]]
			std::string ToString() const;
		};

		struct StringConstant final {
			using String = CompilerDataTypes::String;

			String         m_String;
			ConstantPrefix m_Prefix;

			bool operator==(StringConstant const &other) const {
				return m_String == other.m_String && m_Prefix == other.m_Prefix;
			}

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

		struct Token final {
			using Miscellaneous = char;

			enum class GenericType { Identifier, PpNumber, ChararacterConstant, StringConstant, Miscellaneous };

			using Type = std::variant<GenericType, Punctuator, Keyword, Directive, SpecialPurpose, Miscellaneous>;

			using Value = std::variant<
			        IncludeDirective, Identifier, PpNumber, CharacterConstant, StringConstant, Punctuator, Keyword,
			        Directive, SpecialPurpose, Miscellaneous>;

			[[nodiscard]]
			Type GetValueType() const;

			Value m_Value{SpecialPurpose::Error};
			Span  m_Span;

			friend std::ostream &operator<<(std::ostream &os, Token const &token);

			[[nodiscard]]
			bool IsSpecialPurposeKind(SpecialPurpose specialPurpose) const noexcept;

			[[nodiscard]]
			bool IsPunctuatorKind(Punctuator punctuator) const noexcept;

			[[nodiscard]]
			bool IsTerminating() const noexcept;

			[[nodiscard]]
			bool
			operator==(Token const &) const;
		};

	private:
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

		std::istream                &m_IStream;
		CharStream                   m_Current;
		Diagnosis::Vec              &m_Diagnoses;
		SpanMarker                   m_SubTokenSpanStart{};
		SpanMarker                   m_TokenSpanStart{};
		std::streamoff               m_CurrentTokenLineStart{};
		bool                         m_IsPrecededByNewline{true};
		std::shared_ptr<std::string> m_FileName;

		static constexpr size_t NUM_DIGITS_OCTAL_ESCAPE{3};

		enum class ValidEscapeBase {
			Octal,
			Hexadecimal,
		};

		void SaveSubTokenSpanMarker() noexcept;

		void SaveTokenSpanMarker() noexcept;

		[[nodiscard]]
		Span GetCustomSpan() const noexcept;

		[[nodiscard]]
		Span GetCurrentCharSpan() const noexcept;

		[[nodiscard]]
		Span GetCurrentTokenSpan() const noexcept;

		[[nodiscard]]
		Token MakeToken(Token::Value &&value) const;

		std::optional<CompilerDataTypes::Char> TokenizeNumericalEscapeSequence(ValidEscapeBase base);

		enum class ConstantType {
			Character,
			String,
		};

		[[nodiscard]]
		std::optional<char> TokenizeEscapeSequence();

		[[nodiscard]]
		Tokenizer::Token::Value TokenizeCharacterOrStringLiteral(ConstantPrefix prefix, Tokenizer::ConstantType type);

		[[nodiscard]]
		Tokenizer::Token::Value TokenizeDot();

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
		Tokenizer::Token::Value TokenizePercent();

		[[nodiscard]]
		Tokenizer::Token::Value TokenizeHashHashDigraph();

		[[nodiscard]]
		Punctuator TokenizeCaret();

		[[nodiscard]]
		Tokenizer::Token::Value TokenizeSlash();

		[[nodiscard]]
		Punctuator TokenizeColon();

		[[nodiscard]]
		Tokenizer::Punctuator TokenizeHash();

		[[nodiscard]]
		Tokenizer::Token::Value TokenizePunctuator();

		[[nodiscard]]
		static bool IsNonDigit(char c) noexcept;

		[[nodiscard]]
		static bool IsValidPpNumberCharacter(char c) noexcept;

		[[nodiscard]]
		std::optional<Tokenizer::Token::Value> TokenizePpNumber();

		[[nodiscard]]
		Tokenizer::Token::Value TokenizeHeaderName();

		[[nodiscard]]
		std::optional<Tokenizer::Token::Value> TokenizeDirective();

		enum class UniversalCharacterNameType {
			u,
			U,
		};

		[[nodiscard]]
		std::optional<int> TokenizeHexDigit();

		[[nodiscard]]
		std::optional<char32_t> TokenizeUniversalCharacterName(UniversalCharacterNameType type);

		[[nodiscard]]
		Token::Value TokenizeIdentifierOrKeyword();

		[[nodiscard]]
		Tokenizer::Token::Value Tokenize();

	public:
		Tokenizer(std::string const &fileName, std::istream &iStream, Diagnosis::Vec &diagnoses)
		    : m_IStream{iStream}
		    , m_Current{iStream}
		    , m_Diagnoses{diagnoses}
		    , m_FileName{std::make_shared<std::string>(fileName)} {
			m_Current.Next();
		}

		[[nodiscard]]
		std::string const &GetFileName() const noexcept {
			return *m_FileName;
		}

		[[nodiscard]]
		static constexpr std::string_view KeywordAsIdentString(Keyword keyword) noexcept {
			switch (keyword) {
				case Keyword::Auto:
					return "auto";
				case Keyword::Break:
					return "break";
				case Keyword::Case:
					return "case";
				case Keyword::Char:
					return "char";
				case Keyword::Const:
					return "const";
				case Keyword::Continue:
					return "continue";
				case Keyword::Default:
					return "default";
				case Keyword::Do:
					return "do";
				case Keyword::Double:
					return "double";
				case Keyword::Else:
					return "else";
				case Keyword::Enum:
					return "enum";
				case Keyword::Extern:
					return "extern";
				case Keyword::Float:
					return "float";
				case Keyword::For:
					return "for";
				case Keyword::Goto:
					return "goto";
				case Keyword::If:
					return "if";
				case Keyword::Inline:
					return "inline";
				case Keyword::Int:
					return "int";
				case Keyword::Long:
					return "long";
				case Keyword::Register:
					return "register";
				case Keyword::Restrict:
					return "restrict";
				case Keyword::Return:
					return "return";
				case Keyword::Short:
					return "short";
				case Keyword::Signed:
					return "signed";
				case Keyword::Sizeof:
					return "sizeof";
				case Keyword::Static:
					return "static";
				case Keyword::Struct:
					return "struct";
				case Keyword::Switch:
					return "switch";
				case Keyword::Typedef:
					return "typedef";
				case Keyword::Union:
					return "union";
				case Keyword::Unsigned:
					return "unsigned";
				case Keyword::Void:
					return "void";
				case Keyword::Volatile:
					return "volatile";
				case Keyword::While:
					return "while";
				case Keyword::Alignas:
					return "_Alignas";
				case Keyword::Alignof:
					return "_Alignof";
				case Keyword::Atomic:
					return "_Atomic";
				case Keyword::Bool:
					return "_Bool";
				case Keyword::Complex:
					return "_Complex";
				case Keyword::Generic:
					return "_Generic";
				case Keyword::Imaginary:
					return "_Imaginary";
				case Keyword::Noreturn:
					return "_Noreturn";
				case Keyword::StaticAssert:
					return "_Static_assert";
				case Keyword::ThreadLocal:
					return "_Thread_local";
				default:
					return "";// Empty string for unknown keywords
			}
		}

		[[nodiscard]]
		static constexpr std::string_view PunctuatorToString(Tokenizer::Punctuator punctuator) noexcept;

		[[nodiscard]]
		static StringConstant::String TokenToString(Tokenizer::Token::Value const &tokenValue);

		           Tokenizer(Tokenizer const &) = delete;
		           Tokenizer(Tokenizer &&)      = delete;
		Tokenizer &operator=(Tokenizer const &) = delete;
		Tokenizer &operator=(Tokenizer &&)      = delete;

		Token operator()();
	};

	std::ostream &operator<<(std::ostream &os, Tokenizer::SpecialPurpose specialPurpose);

	std::ostream &operator<<(std::ostream &os, Tokenizer::Punctuator punctuator);

	std::ostream &operator<<(std::ostream &os, Tokenizer::Keyword keyword);

	std::ostream &operator<<(std::ostream &os, Tokenizer::Directive directive);
}// namespace jcc

#endif//JCC_TOKENIZER_H
