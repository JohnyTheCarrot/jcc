#include "tokenizer/token.h"

#include <magic_enum/magic_enum.hpp>

#include "misc/compiler_data_types.h"

namespace jcc::tokenizer {
    bool IncludeDirective::operator==(IncludeDirective const &other) const {
        return m_Name == other.m_Name;
    }

    void PrintTo(IncludeDirective const &headerName, std::ostream *os) {
        *os << "IncludeDirective("
            << magic_enum::enum_name(headerName.m_HeaderType) << ", "
            << headerName.m_Name << ')';
    }

    bool Identifier::IsValidFirstChar(char c) {
        return std::isalpha(c) || c == '_' ||
               c == '\\';// Backslash allows for universal character names
    }

    bool Identifier::IsValidChar(char c) {
        return std::isalnum(c) || c == '_' ||
               c == '\\';// Backslash allows for universal character names
    }

    bool Identifier::operator==(Identifier const &other) const {
        return m_Name == other.m_Name;
    }

    void PrintTo(Identifier const &identifier, std::ostream *os) {
        *os << "Ident(" << identifier.m_Name << ')';
    }

    bool PpNumber::operator==(PpNumber const &other) const {
        return m_Number == other.m_Number;
    }

    std::pair<
            compiler_data_types::Char32::type,
            compiler_data_types::Char32::type>
    GetConstantPrefixRange(ConstantPrefix prefix) {
        switch (prefix) {
            case ConstantPrefix::None:
            case ConstantPrefix::u8:
                return std::pair{
                        compiler_data_types::Char::min(),
                        compiler_data_types::Char::max()
                };
            case ConstantPrefix::L:
                return std::pair{
                        compiler_data_types::WChar::min(),
                        compiler_data_types::WChar::max()
                };
            case ConstantPrefix::u:
                return std::pair{
                        compiler_data_types::Char16::min(),
                        compiler_data_types::Char16::max()
                };
            case ConstantPrefix::U:
                return std::pair{
                        compiler_data_types::Char32::min(),
                        compiler_data_types::Char32::max()
                };
            default:
                assert(false);
                return {};
        }
    }

    ConstantPrefix ToConstantPrefix(std::string_view prefix) {
        if (prefix == "L")
            return ConstantPrefix::L;

        if (prefix == "u")
            return ConstantPrefix::u;

        if (prefix == "U")
            return ConstantPrefix::U;

        if (prefix == "u8")
            return ConstantPrefix::u8;

        return ConstantPrefix::None;
    }

    char const *ConstantPrefixToString(ConstantPrefix prefix) noexcept {
        switch (prefix) {
            case ConstantPrefix::None:
                return "";
            case ConstantPrefix::L:
                return "L";
            case ConstantPrefix::u:
                return "u";
            case ConstantPrefix::U:
                return "U";
            case ConstantPrefix::u8:
                return "u8";
            default:
                assert(false);
                return "";
        }
    }

    void PrintTo(PpNumber const &ppNumber, std::ostream *os) {
        *os << "PpNumber(" << ppNumber.m_Number << ')';
    }

    bool CharacterConstant::operator==(CharacterConstant const &other) const {
        return m_Character == other.m_Character && m_Prefix == other.m_Prefix;
    }

    void PrintTo(CharacterConstant const &characterConstant, std::ostream *os) {
        if (characterConstant.m_Prefix != ConstantPrefix::None)
            *os << ConstantPrefixToString(characterConstant.m_Prefix);

        if (characterConstant.m_Character <=
                    static_cast<std::uint32_t>(compiler_data_types::Char::max()
                    ) &&
            characterConstant.m_Character > ' ') {
            *os << '\'' << static_cast<char>(characterConstant.m_Character)
                << '\'';
            return;
        }

        *os << "'\\u" << std::hex << characterConstant.m_Character << '\'';
    }

    std::string CharacterConstant::ToString() const {
        std::stringstream ss;
        PrintTo(*this, &ss);
        return ss.str();
    }

    bool StringConstant::operator==(StringConstant const &other) const {
        return m_String == other.m_String && m_Prefix == other.m_Prefix;
    }

    std::string StringConstant::ToString() const {
        std::stringstream ss;
        PrintTo(*this, &ss);
        return ss.str();
    }

    int Token::GetTokenTypeId() const {
        auto const type{GetValueType()};

        if (std::holds_alternative<GenericType>(type))
            return static_cast<int>(std::get<GenericType>(type));

        if (std::holds_alternative<Punctuator>(type))
            return static_cast<int>(std::get<Punctuator>(type));

        if (std::holds_alternative<Keyword>(type))
            return static_cast<int>(std::get<Keyword>(type));

        if (std::holds_alternative<Directive>(type))
            return static_cast<int>(std::get<Directive>(type));

        if (std::holds_alternative<SpecialPurpose>(type))
            return static_cast<int>(std::get<SpecialPurpose>(type));

        assert(false);
        return -1;
    }

    bool Token::IsTerminal() const {
        if (!std::holds_alternative<SpecialPurpose>(m_Value))
            return false;

        auto const token{std::get<SpecialPurpose>(m_Value)};

        return token == SpecialPurpose::EndOfFile;
    }

    Token::Type Token::GetValueType() const {
        if (std::holds_alternative<Identifier>(m_Value))
            return GenericType::Identifier;

        if (std::holds_alternative<PpNumber>(m_Value))
            return GenericType::PpNumber;

        if (std::holds_alternative<CharacterConstant>(m_Value))
            return GenericType::ChararacterConstant;

        if (std::holds_alternative<StringConstant>(m_Value))
            return GenericType::StringConstant;

        if (std::holds_alternative<Punctuator>(m_Value))
            return std::get<Punctuator>(m_Value);

        if (std::holds_alternative<Keyword>(m_Value))
            return std::get<Keyword>(m_Value);

        if (std::holds_alternative<Directive>(m_Value))
            return std::get<Directive>(m_Value);

        if (std::holds_alternative<SpecialPurpose>(m_Value))
            return std::get<SpecialPurpose>(m_Value);

        assert(false);
        return SpecialPurpose::Error;
    }

    bool Token::operator==(Token const &other) const {
        return m_Value == other.m_Value && m_Span == other.m_Span;
    }

    void Token::DebugPrintTo(std::ostream &os) const {
        os << '[';
        PrintTo(m_Span, &os);
        os << "]: ";

        if (std::holds_alternative<IncludeDirective>(m_Value)) {
            PrintTo(std::get<IncludeDirective>(m_Value), &os);
        } else if (std::holds_alternative<Identifier>(m_Value)) {
            PrintTo(std::get<Identifier>(m_Value), &os);
        } else if (std::holds_alternative<CharacterConstant>(m_Value)) {
            PrintTo(std::get<CharacterConstant>(m_Value), &os);
        } else if (std::holds_alternative<StringConstant>(m_Value)) {
            PrintTo(std::get<StringConstant>(m_Value), &os);
        } else if (std::holds_alternative<Punctuator>(m_Value)) {
            os << magic_enum::enum_name(std::get<Punctuator>(m_Value));
        } else if (std::holds_alternative<Keyword>(m_Value)) {
            os << magic_enum::enum_name(std::get<Keyword>(m_Value));
        } else if (std::holds_alternative<Directive>(m_Value)) {
            os << magic_enum::enum_name(std::get<Directive>(m_Value));
        } else if (std::holds_alternative<SpecialPurpose>(m_Value)) {
            os << magic_enum::enum_name(std::get<SpecialPurpose>(m_Value));
        } else if (std::holds_alternative<PpNumber>(m_Value)) {
            PrintTo(std::get<PpNumber>(m_Value), &os);
        } else {
            assert(false);
        }
    }

    void PrintTo(Keyword keyword, std::ostream *os) {
        *os << "Keyword::" << magic_enum::enum_name(keyword);
    }

    void PrintTo(Punctuator punctuator, std::ostream *os) {
        *os << "Punctuator::" << magic_enum::enum_name(punctuator);
    }

    void PrintTo(Directive directive, std::ostream *os) {
        *os << "Directive::" << magic_enum::enum_name(directive);
    }

    void PrintTo(SpecialPurpose specialPurpose, std::ostream *os) {
        *os << "SpecialPurpose::" << magic_enum::enum_name(specialPurpose);
    }

    void PrintTo(ConstantPrefix constantPrefix, std::ostream *os) {
        *os << "CharacterConstant::" << magic_enum::enum_name(constantPrefix);
    }

    char const *KeywordToString(Keyword keyword) noexcept {
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

    char const *PunctuatorToString(Punctuator punctuator) noexcept {
        switch (punctuator) {
            case Punctuator::LeftBracket:
                return "[";
            case Punctuator::RightBracket:
                return "]";
            case Punctuator::LeftParenthesis:
                return "(";
            case Punctuator::RightParenthesis:
                return ")";
            case Punctuator::LeftBrace:
                return "{";
            case Punctuator::RightBrace:
                return "}";
            case Punctuator::Dot:
                return ".";
            case Punctuator::Arrow:
                return "->";
            case Punctuator::PlusPlus:
                return "++";
            case Punctuator::MinusMinus:
                return "--";
            case Punctuator::Ampersand:
                return "&";
            case Punctuator::Asterisk:
                return "*";
            case Punctuator::Plus:
                return "+";
            case Punctuator::Minus:
                return "-";
            case Punctuator::Tilde:
                return "~";
            case Punctuator::ExclamationMark:
                return "!";
            case Punctuator::Slash:
                return "/";
            case Punctuator::Percent:
                return "%";
            case Punctuator::LessThan:
                return "<";
            case Punctuator::GreaterThan:
                return ">";
            case Punctuator::LessThanEqual:
                return "<=";
            case Punctuator::GreaterThanEqual:
                return ">=";
            case Punctuator::EqualEqual:
                return "==";
            case Punctuator::ExclamationMarkEqual:
                return "!=";
            case Punctuator::Caret:
                return "^";
            case Punctuator::VerticalBar:
                return "|";
            case Punctuator::AmpersandAmpersand:
                return "&&";
            case Punctuator::VerticalBarVerticalBar:
                return "||";
            case Punctuator::QuestionMark:
                return "?";
            case Punctuator::Colon:
                return ":";
            case Punctuator::Semicolon:
                return ";";
            case Punctuator::Ellipsis:
                return "...";
            case Punctuator::Equal:
                return "=";
            case Punctuator::AsteriskEqual:
                return "*=";
            case Punctuator::SlashEqual:
                return "/=";
            case Punctuator::PercentEqual:
                return "%=";
            case Punctuator::PlusEqual:
                return "+=";
            case Punctuator::LessThanLessThan:
                return "<<";
            case Punctuator::GreaterThanGreaterThan:
                return ">>";
            case Punctuator::MinusEqual:
                return "-=";
            case Punctuator::LessThanLessThanEqual:
                return "<<=";
            case Punctuator::GreaterThanGreaterThanEqual:
                return ">>=";
            case Punctuator::AmpersandEqual:
                return "&=";
            case Punctuator::CaretEqual:
                return "^=";
            case Punctuator::VerticalBarEqual:
                return "|=";
            case Punctuator::Comma:
                return ",";
            case Punctuator::Hash:
                return "#";
            case Punctuator::HashHash:
                return "##";
            case Punctuator::PpLeftParenthesis:
                return "(";
            case Punctuator::Max:
                assert(false);
                break;
        }

        assert(false);
        return "";
    }

    char const *DirectiveToString(Directive directive) noexcept {
        switch (directive) {
            case Directive::Define:
                return "#define";
            case Directive::Include:
                return "#include";
            case Directive::If:
                return "#if";
            case Directive::Elif:
                return "#elif";
            case Directive::Else:
                return "#else";
            case Directive::Endif:
                return "#endif";
            case Directive::Ifdef:
                return "#ifdef";
            case Directive::Ifndef:
                return "#ifndef";
            case Directive::Undef:
                return "#undef";
            case Directive::Line:
                return "#line";
            case Directive::Error:
                return "#error";
            case Directive::Pragma:
                return "#pragma";
            case Directive::Warning:
                return "#warning";
            case Directive::Elifdef:
                return "#elifdef";
            case Directive::Elifndef:
                return "#elifndef";
            case Directive::Max:
                assert(false);
                break;
        }

        assert(false);
        return "";
    }

    void PrintTo(StringConstant const &stringConstant, std::ostream *os) {
        if (stringConstant.m_Prefix != ConstantPrefix::None)
            *os << ConstantPrefixToString(stringConstant.m_Prefix);

        *os << testing::PrintToString(stringConstant.m_String);
    }

    std::ostream &operator<<(std::ostream &os, Token const &token) {
        if (std::holds_alternative<Keyword>(token.m_Value)) {
            os << KeywordToString(std::get<Keyword>(token.m_Value));
        } else if (std::holds_alternative<StringConstant>(token.m_Value)) {
            os << std::get<StringConstant>(token.m_Value).ToString();
        } else if (std::holds_alternative<CharacterConstant>(token.m_Value)) {
            os << std::get<CharacterConstant>(token.m_Value).ToString();
        } else if (std::holds_alternative<PpNumber>(token.m_Value)) {
            os << std::get<PpNumber>(token.m_Value).m_Number;
        } else if (std::holds_alternative<Identifier>(token.m_Value)) {
            os << std::get<Identifier>(token.m_Value).m_Name;
        } else if (std::holds_alternative<Punctuator>(token.m_Value)) {
            os << PunctuatorToString(std::get<Punctuator>(token.m_Value));
        } else if (std::holds_alternative<Directive>(token.m_Value)) {
            os << DirectiveToString(std::get<Directive>(token.m_Value));
        } else if (std::holds_alternative<SpecialPurpose>(token.m_Value)) {
            os << magic_enum::enum_name(std::get<SpecialPurpose>(token.m_Value)
            );
        } else
            assert(false);

        return os;
    }
}// namespace jcc::tokenizer
