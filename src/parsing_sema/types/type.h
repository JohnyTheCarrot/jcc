#ifndef TYPE_H
#define TYPE_H

#include <llvm/IR/Type.h>
#include <ostream>
#include <variant>

namespace llvm {
    class Value;
}

namespace jcc::parsing_sema::types {
    namespace int_ranks {
        using IntegerConversionRank = int;

        constexpr IntegerConversionRank c_BoolRank{0};
        constexpr IntegerConversionRank c_CharRank{c_BoolRank + 1};
        constexpr IntegerConversionRank c_ShortRank{c_CharRank + 1};
        constexpr IntegerConversionRank c_IntRank{c_ShortRank + 1};
        constexpr IntegerConversionRank c_LongRank{c_IntRank + 1};
        constexpr IntegerConversionRank c_LongLongRank{c_LongRank + 1};
    }// namespace int_ranks

    class BitInteger final {
        int m_BitWidth;

    public:
        explicit BitInteger(int bitWidth);

        [[nodiscard]]
        int GetBitWidth() const noexcept;

        bool operator==(BitInteger const &) const = default;

        [[nodiscard]]
        llvm::Type *GetLLVMType() const;

        [[nodiscard]]
        int_ranks::IntegerConversionRank GetConversionRank() const;
    };

    void PrintTo(BitInteger const &bitInteger, std::ostream *os);

    enum class StandardIntegerType {
        Bool     = int_ranks::c_BoolRank,
        Char     = int_ranks::c_CharRank,
        Short    = int_ranks::c_ShortRank,
        Int      = int_ranks::c_IntRank,
        Long     = int_ranks::c_LongRank,
        LongLong = int_ranks::c_LongLongRank,
    };

    void PrintTo(StandardIntegerType const &bitInteger, std::ostream *os);

    struct IntegerType final {
        enum class Signedness {
            Signed,
            Unsigned,
            Unspecified,
        };
        using Type = std::variant<StandardIntegerType, BitInteger>;

        Type       m_Type;
        Signedness m_Sign;

        [[nodiscard]]
        bool operator==(IntegerType const &other) const;

        [[nodiscard]]
        bool IsSigned() const noexcept;

        [[nodiscard]]
        static llvm::Type *GetLLVMType(Type type);

        [[nodiscard]]
        llvm::Type *GetLLVMType() const;

        [[nodiscard]]
        int_ranks::IntegerConversionRank GetConversionRank() const;

        [[nodiscard]]
        IntegerType Promote() const;

        [[nodiscard]]
        bool CanRepresentValuesOf(IntegerType const &other) const;

        [[nodiscard]]
        IntegerType UsualArithmeticConversion(IntegerType const &other) const;
    };

    void PrintTo(IntegerType type, std::ostream *os);

    class ValueType final {
    public:
        using Type = std::variant<IntegerType>;

        explicit ValueType(Type &&type);

        [[nodiscard]]
        bool IsArithmetic() const noexcept;

        [[nodiscard]]
        bool IsInteger() const noexcept;

        [[nodiscard]]
        llvm::Type *GetLLVMType() const;

        [[nodiscard]]
        Type const &GetInnerType() const noexcept;

        [[nodiscard]]
        bool operator==(ValueType const &other) const;

    private:
        Type m_Type;
    };

    [[nodiscard]]
    llvm::Value *
    CastValue(llvm::Value *value, ValueType const &from, ValueType const &to);

    void PrintTo(ValueType const &type, std::ostream *os);

    [[nodiscard]]
    ValueType
    UsualArithmeticConversions(ValueType const &lhs, ValueType const &rhs);
}// namespace jcc::parsing_sema::types

#endif//TYPE_H
