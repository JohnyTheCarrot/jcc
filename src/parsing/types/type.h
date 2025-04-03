#ifndef TYPE_H
#define TYPE_H

#include <llvm/IR/Type.h>
#include <variant>

namespace llvm {
    class Value;
}

namespace jcc::parsing {
    class TypeSpecifier;
}

namespace jcc::parsing::types {
#pragma region Floats
    // Ensure that the enum values indicate the rank of the floating type
    enum class StandardFloatingType { Float = 0, Double = 1, LongDouble = 2 };

    void PrintTo(StandardFloatingType floatingType, std::ostream *os);

    class FloatingType final {
    public:
        using Type = std::variant<StandardFloatingType>;

        explicit FloatingType(Type &&specifier);

        [[nodiscard]]
        llvm::Type *GetLLVMType() const;

        [[nodiscard]]
        bool operator==(FloatingType const &other) const;

        [[nodiscard]]
        Type GetType() const noexcept;

        [[nodiscard]]
        FloatingType UsualArithmeticConversion(FloatingType const &other) const;

    private:
        Type m_Type;
    };

    void PrintTo(FloatingType const &type, std::ostream *os);
#pragma endregion

#pragma region Ints

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

    class IntegerType final {
    public:
        enum class Signedness {
            Signed,
            Unsigned,
            Unspecified,
        };
        using Type = std::variant<StandardIntegerType, BitInteger>;

        IntegerType(Type type, Signedness sign);

        [[nodiscard]]
        Type GetType() const noexcept;

        [[nodiscard]]
        Signedness GetSignedness() const noexcept;

        [[nodiscard]]
        bool operator==(IntegerType const &other) const;

        [[nodiscard]]
        bool IsSigned() const noexcept;

        [[nodiscard]]
        static llvm::Type *GetLLVMType(Type type);

        [[nodiscard]]
        llvm::Type *GetLLVMType() const;

        [[nodiscard]]
        unsigned GetBitWidth() const;

        [[nodiscard]]
        int_ranks::IntegerConversionRank GetConversionRank() const;

        [[nodiscard]]
        IntegerType Promote() const;

        [[nodiscard]]
        bool CanRepresentValuesOf(IntegerType const &other) const;

        [[nodiscard]]
        IntegerType UsualArithmeticConversion(IntegerType other) const;

        [[nodiscard]]
        FloatingType UsualArithmeticConversion(FloatingType other) const;

    private:
        Type       m_Type;
        Signedness m_Sign;
    };

    void PrintTo(IntegerType type, std::ostream *os);
#pragma endregion

    class ValueType final {
    public:
        using Type = std::variant<IntegerType, FloatingType>;

        explicit ValueType(Type &&type);

        explicit ValueType(TypeSpecifier const &typeSpecifiers);

        [[nodiscard]]
        bool IsArithmetic() const noexcept;

        [[nodiscard]]
        bool IsInteger() const noexcept;

        [[nodiscard]]
        bool IsFloating() const noexcept;

        [[nodiscard]]
        bool IsReal() const noexcept;

        [[nodiscard]]
        bool IsScalar() const noexcept;

        [[nodiscard]]
        llvm::Type *GetLLVMType() const;

        [[nodiscard]]
        Type const &GetType() const noexcept;

        [[nodiscard]]
        bool operator==(ValueType const &other) const;

        [[nodiscard]]
        std::string ToString() const;

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
}// namespace jcc::parsing::types

#endif//TYPE_H
