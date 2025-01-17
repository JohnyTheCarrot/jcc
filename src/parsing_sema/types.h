#ifndef TYPES_H
#define TYPES_H

#include <optional>

#include "ast_node.h"
#include "parser.h"

namespace jcc::tokenizer {
    struct Token;
}

namespace jcc::parsing_sema {
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

    class AstStorageClassSpecifiers final : public AstNode {
        std::vector<StorageClassSpecifier> m_StorageClassSpecifiers;

    public:
        explicit AstStorageClassSpecifiers(
                std::vector<StorageClassSpecifier> &&specifiers
        );
    };

    enum class BasicTypeSpecifier {
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

    using TypeSpecifier = std::variant<BasicTypeSpecifier>;

    [[nodiscard]]
    bool AreBasicTypesCompatible(BasicTypeSpecifier types);

    /**
     * Combine the current specifier with the given specifier.
     * 
     * @param current The current specifier.
     * @param specifier The specifier to combine with the current specifier.
     * @throws std::invalid_argument If the given specifier is not compatible with the current specifier.
     * @return The combined specifier.
     */
    void CombineTypeSpecifiers(
            TypeSpecifier &current, TypeSpecifier const &specifier
    );
}// namespace jcc::parsing_sema

#endif//TYPES_H
