#ifndef JCC_TOKEN_H
#define JCC_TOKEN_H

#include <iosfwd>                    // for ostream
#include <misc/compiler_data_types.h>// for Char32
#include <string>                    // for string
#include <string_view>               // for string_view
#include <utility>                   // for pair
#include <variant>                   // for variant, holds_alternative

#include "misc/Span.h"// for Span

namespace jcc::tokenizer {
    struct IncludeDirective final {
        std::string m_Name{};

        enum class HeaderType { HChar, QChar, MacroName } m_HeaderType;

        [[nodiscard]]
        bool operator==(IncludeDirective const &other) const;

        friend void
        PrintTo(IncludeDirective const &headerName, std::ostream *os);
    };

    struct Identifier final {
        using IdentString = std::string;
        IdentString m_Name{};

        [[nodiscard]]
        static bool IsValidFirstChar(char c);

        [[nodiscard]]
        static bool IsValidChar(char c);

        [[nodiscard]]
        bool operator==(Identifier const &other) const;

        friend void PrintTo(Identifier const &identifier, std::ostream *os);
    };

    struct PpNumber final {
        // A preprocessing number has neither a value nor a type at the time of preprocessing.
        // It acquires both after a successful conversion to a number token.
        std::string m_Number{};

        [[nodiscard]]
        bool operator==(PpNumber const &other) const;

        friend void PrintTo(PpNumber const &ppNumber, std::ostream *os);
    };

    enum class ConstantPrefix { None, L, u, U, u8 };

    [[nodiscard]]
    std::pair<
            compiler_data_types::Char32::type,
            compiler_data_types::Char32::type>
    GetConstantPrefixRange(ConstantPrefix prefix);

    [[nodiscard]]
    ConstantPrefix ToConstantPrefix(std::string_view prefix);

    [[nodiscard]]
    char const *ConstantPrefixToString(ConstantPrefix prefix) noexcept;

    struct CharacterConstant final {
        compiler_data_types::Char32::type m_Character;
        ConstantPrefix                    m_Prefix{ConstantPrefix::None};

        [[nodiscard]]
        bool operator==(CharacterConstant const &other) const;

        friend void
        PrintTo(CharacterConstant const &characterConstant, std::ostream *os);

        [[nodiscard]]
        std::string ToString() const;
    };

    struct StringConstant final {
        std::string    m_String;
        ConstantPrefix m_Prefix{ConstantPrefix::None};

        [[nodiscard]]
        bool operator==(StringConstant const &other) const;

        friend void
        PrintTo(StringConstant const &stringConstant, std::ostream *os);

        [[nodiscard]]
        std::string ToString() const;
    };

    enum class Punctuator {
        Ampersand,
        AmpersandAmpersand,
        AmpersandEqual,
        Arrow,
        Asterisk,
        AsteriskEqual,
        Caret,
        CaretEqual,
        Colon,
        Comma,
        Dot,
        Ellipsis,
        Equal,
        EqualEqual,
        ExclamationMark,
        ExclamationMarkEqual,
        GreaterThan,
        GreaterThanEqual,
        GreaterThanGreaterThan,
        GreaterThanGreaterThanEqual,
        Hash,
        HashHash,
        LeftBrace,
        LeftBracket,
        LeftParenthesis,
        LessThan,
        LessThanEqual,
        LessThanLessThan,
        LessThanLessThanEqual,
        Minus,
        MinusEqual,
        MinusMinus,
        Percent,
        PercentEqual,
        Plus,
        PlusEqual,
        PlusPlus,
        PpLeftParenthesis,
        QuestionMark,
        RightBrace,
        RightBracket,
        RightParenthesis,
        Semicolon,
        Slash,
        SlashEqual,
        Tilde,
        VerticalBar,
        VerticalBarEqual,
        VerticalBarVerticalBar,
    };

    enum class Keyword {
        Alignas,
        Alignof,
        Atomic,
        Auto,
        BitInt,
        Bool,
        Break,
        Case,
        Char,
        Complex,
        Const,
        Constexpr,
        Continue,
        Decimal32,
        Decimal64,
        Decimal128,
        Default,
        Do,
        Double,
        Else,
        Enum,
        Extern,
        False,
        Float,
        For,
        Generic,
        Goto,
        If,
        Imaginary,
        Inline,
        Int,
        Long,
        Noreturn,
        Nullptr,
        Register,
        Restrict,
        Return,
        Short,
        Signed,
        Sizeof,
        Static,
        StaticAssert,
        Struct,
        Switch,
        ThreadLocal,
        True,
        Typedef,
        Union,
        Unsigned,
        Void,
        Volatile,
        While,
    };

    enum class Directive {
        Define,
        Elif,
        Elifdef,
        Elifndef,
        Else,
        Endif,
        Error,
        If,
        Ifdef,
        Ifndef,
        Include,
        Line,
        Pragma,
        Undef,
        Warning,
    };

    enum class SpecialPurpose {
        EndOfFile,
        NewLine,
        LineComment,
        BlockComment,
        InvalidEscape,
    };

    struct Token {
        enum class GenericType {
            Identifier,
            PpNumber,
            ChararacterConstant,
            StringConstant
        };

        using Type = std::variant<
                GenericType, Punctuator, Keyword, Directive, SpecialPurpose>;

        using Value = std::variant<
                IncludeDirective, Identifier, PpNumber, CharacterConstant,
                StringConstant, Punctuator, Keyword, Directive, SpecialPurpose>;

        Value m_Value;
        Span  m_Span;

        [[nodiscard]]
        bool IsTerminal() const;

        [[nodiscard]]
        Type GetValueType() const;

        template<typename T>
        [[nodiscard]]
        bool Is() const {
            return std::holds_alternative<T>(m_Value);
        }

        template<typename T>
        [[nodiscard]]
        bool Is(T const &value) const {
            return Is<T>() && std::get<T>(m_Value) == value;
        }

        [[nodiscard]]
        bool operator==(Token const &other) const;

        friend std::ostream &operator<<(std::ostream &os, Token const &token);

        [[nodiscard]]
        std::string ToString() const;

        void DebugPrintTo(std::ostream &os) const;
    };

    void PrintTo(Keyword keyword, std::ostream *os);

    void PrintTo(Punctuator punctuator, std::ostream *os);

    void PrintTo(Directive directive, std::ostream *os);

    void PrintTo(SpecialPurpose specialPurpose, std::ostream *os);

    void PrintTo(ConstantPrefix constantPrefix, std::ostream *os);

    [[nodiscard]]
    char const *KeywordToString(Keyword keyword) noexcept;

    [[nodiscard]]
    char const *PunctuatorToString(Punctuator punctuator) noexcept;

    [[nodiscard]]
    char const *DirectiveToString(Directive directive) noexcept;
}// namespace jcc::tokenizer

#endif//JCC_TOKEN_H
