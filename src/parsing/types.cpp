#include "types.h"

#include <functional>               // for hash, equal_to
#include <magic_enum/magic_enum.hpp>// for operator|, operator|=
#include <ostream>                  // for basic_ios, basic_ostream
#include <string>                   // for allocator, operator<<
#include <unordered_set>            // for unordered_set

#include "magic_enum/magic_enum_flags.hpp"// for enum_flags_name
#include "tokenizer/token.h"              // for Keyword, Token

namespace jcc::parsing {
    std::optional<StorageClassSpecifier>
    ParseStorageClassSpecifier(tokenizer::Token const &token) noexcept {
        if (!token.Is<tokenizer::Keyword>())
            return std::nullopt;

        switch (std::get<tokenizer::Keyword>(token.m_Value)) {
            case tokenizer::Keyword::Auto:
                return StorageClassSpecifier::Auto;
            case tokenizer::Keyword::Constexpr:
                return StorageClassSpecifier::Constexpr;
            case tokenizer::Keyword::Extern:
                return StorageClassSpecifier::Extern;
            case tokenizer::Keyword::Register:
                return StorageClassSpecifier::Register;
            case tokenizer::Keyword::Static:
                return StorageClassSpecifier::Static;
            case tokenizer::Keyword::ThreadLocal:
                return StorageClassSpecifier::ThreadLocal;
            case tokenizer::Keyword::Typedef:
                return StorageClassSpecifier::Typedef;
            default:
                return std::nullopt;
        }
    }

    AstStorageClassSpecifiers::AstStorageClassSpecifiers(
            std::vector<StorageClassSpecifier> &&specifiers
    )
        : m_StorageClassSpecifiers{std::move(specifiers)} {
    }

    void PrintTo(BasicTypeSpecifier specifier, std::ostream *os) {
        *os << magic_enum::enum_flags_name(specifier);
    }

    TypeSpecifier::TypeSpecifier(BasicTypeSpecifier specifier) noexcept
        : m_BasicSpecifiers{specifier} {
    }

    using namespace magic_enum::bitwise_operators;

    std::unordered_set const c_ValidBasicTypeSpecifiers{
            // void
            BasicTypeSpecifier::Void,
            // char
            BasicTypeSpecifier::Char,
            // signed char
            BasicTypeSpecifier::Char | BasicTypeSpecifier::Signed,
            // unsigned char
            BasicTypeSpecifier::Char | BasicTypeSpecifier::Unsigned,
            // short, signed short , short int, or signed short int
            BasicTypeSpecifier::Short,
            BasicTypeSpecifier::Short | BasicTypeSpecifier::Signed,
            BasicTypeSpecifier::Short | BasicTypeSpecifier::Int,
            BasicTypeSpecifier::Short | BasicTypeSpecifier::Signed |
                    BasicTypeSpecifier::Int,
            // unsigned short, or unsigned short int
            BasicTypeSpecifier::Short | BasicTypeSpecifier::Unsigned,
            BasicTypeSpecifier::Short | BasicTypeSpecifier::Unsigned |
                    BasicTypeSpecifier::Int,
            // int, signed, or signed int
            BasicTypeSpecifier::Int, BasicTypeSpecifier::Signed,
            BasicTypeSpecifier::Int | BasicTypeSpecifier::Signed,
            // unsigned, or unsigned int
            BasicTypeSpecifier::Unsigned,
            BasicTypeSpecifier::Unsigned | BasicTypeSpecifier::Int,
            // long, signed long, long int, or signed long int
            BasicTypeSpecifier::Long,
            BasicTypeSpecifier::Long | BasicTypeSpecifier::Signed,
            BasicTypeSpecifier::Long | BasicTypeSpecifier::Int,
            BasicTypeSpecifier::Long | BasicTypeSpecifier::Signed |
                    BasicTypeSpecifier::Int,
            // unsigned long, or unsigned long int
            BasicTypeSpecifier::Long | BasicTypeSpecifier::Unsigned,
            BasicTypeSpecifier::Long | BasicTypeSpecifier::Unsigned |
                    BasicTypeSpecifier::Int,
            // long long, signed long long, long long int, or signed long long int
            BasicTypeSpecifier::LongLong,
            BasicTypeSpecifier::LongLong | BasicTypeSpecifier::Signed,
            BasicTypeSpecifier::LongLong | BasicTypeSpecifier::Int,
            BasicTypeSpecifier::LongLong | BasicTypeSpecifier::Signed |
                    BasicTypeSpecifier::Int,
            // unsigned long long, or unsigned long long int
            BasicTypeSpecifier::LongLong | BasicTypeSpecifier::Unsigned,
            BasicTypeSpecifier::LongLong | BasicTypeSpecifier::Unsigned |
                    BasicTypeSpecifier::Int,
            // _BitInt, signed _BitInt
            BasicTypeSpecifier::BitInt,
            BasicTypeSpecifier::BitInt | BasicTypeSpecifier::Signed,
            // unsigned _BitInt
            BasicTypeSpecifier::BitInt | BasicTypeSpecifier::Unsigned,
            // float
            BasicTypeSpecifier::Float,
            // double
            BasicTypeSpecifier::Double,
            // long double
            BasicTypeSpecifier::LongDouble,
            // _Decimal32, _Decimal64, _Decimal128
            BasicTypeSpecifier::Decimal32, BasicTypeSpecifier::Decimal64,
            BasicTypeSpecifier::Decimal128,
            // _Bool
            BasicTypeSpecifier::Bool,
            // float _Complex
            BasicTypeSpecifier::Complex | BasicTypeSpecifier::Float,
            // double _Complex
            BasicTypeSpecifier::Complex | BasicTypeSpecifier::Double,
            // long double _Complex
            BasicTypeSpecifier::Complex | BasicTypeSpecifier::LongDouble
    };

    bool TypeSpecifier::IsEmpty() const noexcept {
        if (m_BasicSpecifiers == BasicTypeSpecifier::None)
            return true;

        return false;
    }

    TypeSpecifier &TypeSpecifier::operator|=(BasicTypeSpecifier specifier
    ) noexcept {
        m_BasicSpecifiers = m_BasicSpecifiers | specifier;
        return *this;
    }

    TypeSpecifier &TypeSpecifier::operator|=(TypeSpecifier const &specifier
    ) noexcept {
        m_BasicSpecifiers |= specifier.m_BasicSpecifiers;

        return *this;
    }

    TypeSpecifier TypeSpecifier::operator|(BasicTypeSpecifier specifier
    ) const noexcept {
        TypeSpecifier result{*this};
        result |= specifier;
        return result;
    }

    TypeSpecifier TypeSpecifier::operator|(TypeSpecifier const &specifier
    ) const noexcept {
        TypeSpecifier result{*this};
        result |= specifier;
        return result;
    }

    bool TypeSpecifier::IsValid() const {
        // TODO: Check for invalid combinations of non basic type specifiers.
        return AreBasicTypesCompatible(m_BasicSpecifiers);
    }

    bool AreBasicTypesCompatible(BasicTypeSpecifier types) {
        return c_ValidBasicTypeSpecifiers.contains(types);
    }

    TypeSpecifierQualifier &
    TypeSpecifierQualifier::operator|=(TypeSpecifierQualifier const &other
    ) noexcept {
        m_Specifiers |= other.m_Specifiers;
        m_Qualifiers |= other.m_Qualifiers;
        return *this;
    }

    bool TypeSpecifierQualifier::IsEmpty() const noexcept {
        return m_Specifiers.IsEmpty() && m_Qualifiers == TypeQualifier::None;
    }

    SpecifierQualifierList::SpecifierQualifierList(
            Span &&span, TypeSpecifierQualifier list
    )
        : AstNode{std::move(span)}
        , m_List{list} {
    }

    void SpecifierQualifierList::AcceptOnNode(AstNodeVisitor *visitor) const {
        visitor->Visit(this);
    }

    bool SpecifierQualifierList::IsValid() const noexcept {
        return m_List.m_Specifiers.IsValid();
    }

    std::optional<TypeQualifier>
    ParseTypeQualifier(tokenizer::Token const &token) {
        if (!token.Is<tokenizer::Keyword>())
            return std::nullopt;

        switch (std::get<tokenizer::Keyword>(token.m_Value)) {
            case tokenizer::Keyword::Const:
                return TypeQualifier::Const;
            case tokenizer::Keyword::Restrict:
                return TypeQualifier::Restrict;
            case tokenizer::Keyword::Volatile:
                return TypeQualifier::Volatile;
            case tokenizer::Keyword::Atomic:
                return TypeQualifier::Atomic;
            default:
                return std::nullopt;
        }
    }
}// namespace jcc::parsing
