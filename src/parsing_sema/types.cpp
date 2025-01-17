#include "types.h"

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_flags.hpp>
#include <unordered_set>

#include "tokenizer/token.h"

namespace jcc::parsing_sema {
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

    bool AreBasicTypesCompatible(BasicTypeSpecifier types) {
        return c_ValidBasicTypeSpecifiers.contains(types);
    }

    void CombineTypeSpecifiers(
            TypeSpecifier &current, TypeSpecifier const &specifier
    ) {
        if (!std::holds_alternative<BasicTypeSpecifier>(current) ||
            !std::holds_alternative<BasicTypeSpecifier>(specifier)) {
            throw std::invalid_argument{
                    "The type specifiers must be basic types."
            };
        }

        auto       basicCurrent{std::get<BasicTypeSpecifier>(current)};
        auto const basicSpecifier{std::get<BasicTypeSpecifier>(specifier)};

        basicCurrent |= basicSpecifier;

        if (AreBasicTypesCompatible(basicCurrent)) {
            current = basicCurrent;
            return;
        }

        throw std::invalid_argument{
                "The given type specifier is not compatible with the current "
                "type specifier."
        };
    }
}// namespace jcc::parsing_sema
