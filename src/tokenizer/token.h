#ifndef JCC_TOKEN_H
#define JCC_TOKEN_H

#include <misc/compiler_data_types.h>
#include <string>
#include <utility>
#include <variant>

#include "misc/Span.h"

namespace jcc::tokenizer {
    struct IncludeDirective final {
        std::string m_Name{};

        enum class HeaderType { HChar, QChar, MacroName } m_HeaderType;

        [[nodiscard]]
        bool
        operator==(IncludeDirective const &other) const;

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
    std::pair<
            compiler_data_types::Char32::type,
            compiler_data_types::Char32::type>
    GetConstantPrefixRange(ConstantPrefix prefix);

    [[nodiscard]]
    ConstantPrefix ToConstantPrefix(std::string_view prefix);

    struct CharacterConstant final {
        compiler_data_types::Char32::type m_Character;
        ConstantPrefix                    m_Prefix;

        [[nodiscard]]
        bool
        operator==(CharacterConstant const &other) const;

        friend void
        PrintTo(CharacterConstant const &characterConstant, std::ostream *os);

        [[nodiscard]]
        std::string ToString() const;
    };

    struct StringConstant final {
        std::string    m_String;
        ConstantPrefix m_Prefix;

        [[nodiscard]]
        bool
        operator==(StringConstant const &other) const;

        friend void
        PrintTo(StringConstant const &stringConstant, std::ostream *os);

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
        False,
        True,
        Nullptr,
    };

    enum class Directive {
        Include,
        Define,
        Undef,
        Line,
        Error,
        Pragma,
        If,
        Ifdef,
        Ifndef,
        Elif,
        Else,
        Endif
    };

    enum class SpecialPurpose {
        EndOfFile,
        Error,
        NewLine,
        LineComment,
        BlockComment,
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
        bool Is(T const &value) const {
            return std::holds_alternative<T>(m_Value) &&
                   std::get<T>(m_Value) == value;
        }

        [[nodiscard]]
        bool
        operator==(Token const &other) const;

        friend std::ostream &operator<<(std::ostream &os, Token const &token);
    };

    void PrintTo(Keyword keyword, std::ostream *os);

    void PrintTo(Punctuator punctuator, std::ostream *os);

    void PrintTo(Directive directive, std::ostream *os);

    void PrintTo(SpecialPurpose specialPurpose, std::ostream *os);

    void PrintTo(ConstantPrefix constantPrefix, std::ostream *os);

    [[nodiscard]]
    static constexpr std::string_view KeywordAsIdentString(Keyword keyword
    ) noexcept {
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
}// namespace jcc::tokenizer

#endif//JCC_TOKEN_H
