#ifndef TYPES_H
#define TYPES_H

#include <magic_enum/magic_enum_flags.hpp>
#include <optional>

#include "diagnostics/variants/todo.hpp"
#include "parser.h"

namespace jcc::tokenizer {
    struct Token;
}

namespace jcc::parsing {
    enum class StorageClassSpecifier {
        Auto,
        Constexpr,
        Extern,
        Register,
        Static,
        ThreadLocal,
        Typedef
    };

    [[nodiscard]]
    std::optional<StorageClassSpecifier>
    ParseStorageClassSpecifier(tokenizer::Token const &token) noexcept;

    template<typename TIterator>
        requires TokenIterator<TIterator>
    [[nodiscard]]
    std::vector<StorageClassSpecifier>
    ParseStorageClassSpecifiers(TIterator current, TIterator end) {
        std::vector<StorageClassSpecifier> specifiers{};

        while (current != end) {
            auto const &token{*current};

            if (auto const specifier{ParseStorageClassSpecifier(token)};
                specifier.has_value()) {
                specifiers.push_back(specifier.value());
                ++current;
                continue;
            }

            return specifiers;
        }

        return {};
    }

    class AstStorageClassSpecifiers final {
        std::vector<StorageClassSpecifier> m_StorageClassSpecifiers;

    public:
        explicit AstStorageClassSpecifiers(
                std::vector<StorageClassSpecifier> &&specifiers
        );
    };

    /**
     * Meant to be used as a bitset for a collection of basic type specifiers.
     */
    enum class BasicTypeSpecifier {
        None       = 0,// No type specifier
        Void       = 1 << 0,
        Char       = 1 << 1,
        Short      = 1 << 2,
        Int        = 1 << 3,
        Long       = 1 << 4,
        LongLong   = 1 << 5,
        Float      = 1 << 6,
        Double     = 1 << 7,
        LongDouble = 1 << 8,
        Signed     = 1 << 9,
        Unsigned   = 1 << 10,
        BitInt     = 1 << 11,// C23 _BitInt(N)
        Bool       = 1 << 12,// C99 _Bool
        Complex    = 1 << 13,// C99 _Complex
        Decimal32  = 1 << 14,// TS 18661-2 _Decimal32
        Decimal64  = 1 << 15,// TS 18661-2 _Decimal64
        Decimal128 = 1 << 16 // TS 18661-2 _Decimal128
    };

    void PrintTo(BasicTypeSpecifier specifier, std::ostream *os);

    /**
     * This struct holds the type specifiers for a type.
     */
    class TypeSpecifier final {
        BasicTypeSpecifier m_BasicSpecifiers{BasicTypeSpecifier::None};

    public:
        explicit TypeSpecifier(BasicTypeSpecifier specifier) noexcept;

        [[nodiscard]]
        BasicTypeSpecifier GetBasicSpecifiers() const noexcept {
            return m_BasicSpecifiers;
        }

        [[nodiscard]]
        bool IsEmpty() const noexcept;

        /**
         * Combines the given type specifier with this one.
         * The caller is responsible for ensuring that the combination is valid.
         * 
         * @param specifier The type specifier to combine with this one.
         * @return A reference to this type specifier.
         */
        TypeSpecifier &operator|=(BasicTypeSpecifier specifier) noexcept;

        /**
         * Combines the given type specifier with this one.
         * The caller is responsible for ensuring that the combination is valid.
         * 
         * @param specifier The type specifier to combine with this one.
         * @return A reference to this type specifier.
         */
        TypeSpecifier &operator|=(TypeSpecifier const &specifier) noexcept;

        [[nodiscard]]
        TypeSpecifier operator|(BasicTypeSpecifier specifier) const noexcept;

        [[nodiscard]]
        TypeSpecifier operator|(TypeSpecifier const &specifier) const noexcept;

        [[nodiscard]]
        bool IsValid() const;
    };

    template<typename TIterator>
        requires TokenIterator<TIterator>
    [[nodiscard]]
    std::optional<TypeSpecifier>
    ParseTypeSpecifier(TIterator current, TIterator end) {
        if (current == end)
            return std::nullopt;

        tokenizer::Token const token{*current};

        if (!token.Is<tokenizer::Keyword>())
            return std::nullopt;

        auto specifier{BasicTypeSpecifier::None};
        switch (std::get<tokenizer::Keyword>(token.m_Value)) {
            case tokenizer::Keyword::Void:
                specifier = BasicTypeSpecifier::Void;
                break;
            case tokenizer::Keyword::Char:
                specifier = BasicTypeSpecifier::Char;
                break;
            case tokenizer::Keyword::Short:
                specifier = BasicTypeSpecifier::Short;
                break;
            case tokenizer::Keyword::Int:
                specifier = BasicTypeSpecifier::Int;
                break;
            case tokenizer::Keyword::Long:
                specifier = BasicTypeSpecifier::Long;
                break;
            case tokenizer::Keyword::Float:
                specifier = BasicTypeSpecifier::Float;
                break;
            case tokenizer::Keyword::Double:
                specifier = BasicTypeSpecifier::Double;
                break;
            case tokenizer::Keyword::Signed:
                specifier = BasicTypeSpecifier::Signed;
                break;
            case tokenizer::Keyword::Unsigned:
                specifier = BasicTypeSpecifier::Unsigned;
                break;
            case tokenizer::Keyword::BitInt: {
                specifier = BasicTypeSpecifier::BitInt;
                // TODO: Parse the bit width.
                CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                                token.m_Span.m_Source, token.m_Span.m_Span
                        );
                break;
            }
            case tokenizer::Keyword::Bool:
                specifier = BasicTypeSpecifier::Bool;
                break;
            case tokenizer::Keyword::Complex:
                specifier = BasicTypeSpecifier::Complex;
                break;
            case tokenizer::Keyword::Decimal32:
                specifier = BasicTypeSpecifier::Decimal32;
                break;
            case tokenizer::Keyword::Decimal64:
                specifier = BasicTypeSpecifier::Decimal64;
                break;
            case tokenizer::Keyword::Decimal128:
                specifier = BasicTypeSpecifier::Decimal128;
                break;
            default:
                return std::nullopt;
        }

        return TypeSpecifier{specifier};
    }

    [[nodiscard]]
    bool AreBasicTypesCompatible(BasicTypeSpecifier types);

    enum class TypeQualifier : std::uint8_t {
        None     = 0,// No type qualifier
        Const    = 1 << 0,
        Restrict = 1 << 1,
        Volatile = 1 << 2,
        Atomic   = 1 << 3,
    };

    [[nodiscard]]
    std::optional<TypeQualifier>
    ParseTypeQualifier(tokenizer::Token const &token);

    struct TypeSpecifierQualifier final {
        TypeSpecifier m_Specifiers{BasicTypeSpecifier::None};
        TypeQualifier m_Qualifiers{TypeQualifier::None};

        TypeSpecifierQualifier &operator|=(TypeSpecifierQualifier const &other
        ) noexcept;

        [[nodiscard]]
        bool IsEmpty() const noexcept;
    };

    class SpecifierQualifierList final : public AstNode {
        // The specifiers and qualifiers are stored as a bitset.
        TypeSpecifierQualifier m_List;

    public:
        SpecifierQualifierList(Span &&span, TypeSpecifierQualifier list);

        [[nodiscard]]
        TypeSpecifierQualifier GetTypeSpecifierQualifier() const noexcept {
            return m_List;
        }

        void AcceptOnNode(AstNodeVisitor *visitor) const override;

        [[nodiscard]]
        bool IsValid() const noexcept;
    };

    template<typename TIterator>
        requires TokenIterator<TIterator>
    [[nodiscard]]
    std::optional<TypeSpecifierQualifier>
    ParseTypeSpecifierQualifier(TIterator current, TIterator end) {
        if (auto const specifier{ParseTypeSpecifier(current, end)};
            specifier.has_value()) {
            return TypeSpecifierQualifier{.m_Specifiers = specifier.value()};
        }

        auto const token{*current};

        if (auto const qualifier{ParseTypeQualifier(token)};
            qualifier.has_value()) {
            return TypeSpecifierQualifier{.m_Qualifiers = qualifier.value()};
        }

        if (token.Is(tokenizer::Keyword::Alignas)) {
            CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                            token.m_Span.m_Source, token.m_Span.m_Span
                    );
        }

        return std::nullopt;
    }

    /**
     * Parses a list of specifiers and qualifiers.
     *
     * @note This function does not check for the validity of the specifier and qualifier combinations, that is the responsibility of semantic analysis.
     * @tparam TIterator The type of the iterator.
     * @param current The current iterator position.
     * @param end The end iterator position.
     * @param possiblyFollowedByDeclarator A reference to a boolean that will be set to true if the list is possibly followed by a declarator. This means that a left bracket was found after the last specifier or qualifier and is already consumed.
     * @return A list of specifiers and qualifiers.
     * @note It is the responsibility of the caller to ensure that the iterator is valid and that the range is not empty.
     */
    template<typename TIterator>
        requires TokenIterator<TIterator>
    [[nodiscard]]
    std::optional<SpecifierQualifierList> ParseSpecifierQualifierList(
            TIterator &current, TIterator const &end,
            bool &possiblyFollowedByDeclarator
    ) {
        using namespace magic_enum::bitwise_operators;

        Span span{current->m_Span.m_Source, current->m_Span.m_Span};
        TypeSpecifierQualifier typeSpecifierQualifiers{};

        auto const firstTypeSpecifierQualifier{
                ParseTypeSpecifierQualifier(current, end)
        };

        if (!firstTypeSpecifierQualifier.has_value()) {
            return std::nullopt;
        }

        span += current->m_Span;
        typeSpecifierQualifiers = firstTypeSpecifierQualifier.value();
        ++current;

        while (current != end) {
            auto const typeSpecifierQualifier{
                    ParseTypeSpecifierQualifier(current, end)
            };
            if (!typeSpecifierQualifier.has_value()) {
                break;
            }

            span += current->m_Span;
            typeSpecifierQualifiers |= typeSpecifierQualifier.value();
            ++current;

            if (current == end)
                break;

            if (tokenizer::Token const nextToken{*current};
                !nextToken.Is(tokenizer::Punctuator::LeftBracket)) {
                continue;
            }

            ++current;
            if (current == end)
                break;

            if (tokenizer::Token const nextToken{*current};
                nextToken.Is(tokenizer::Punctuator::LeftBracket)) {
                // TODO: attribute specifier sequence
                CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                                nextToken.m_Span.m_Source,
                                nextToken.m_Span.m_Span
                        );
            }

            possiblyFollowedByDeclarator = true;
            break;
        }

        if (typeSpecifierQualifiers.IsEmpty())
            return std::nullopt;

        return SpecifierQualifierList{std::move(span), typeSpecifierQualifiers};
    }
}// namespace jcc::parsing

#endif//TYPES_H
