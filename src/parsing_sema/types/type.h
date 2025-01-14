#ifndef TYPE_H
#define TYPE_H
#include <ostream>
#include <variant>

namespace jcc::parsing_sema::types {
    class BitInteger final {
        int m_BitWidth;

    public:
        explicit BitInteger(int bitWidth);

        [[nodiscard]]
        int GetBitWidth() const noexcept;

        bool operator==(BitInteger const &) const = default;
    };

    void PrintTo(BitInteger const &bitInteger, std::ostream *os);

    enum class StandardIntegerType {
        Char,
        Short,
        Int,
        Long,
        LongLong,
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
    };

    void PrintTo(IntegerType type, std::ostream *os);

    class IntegerLimits final {
    public:
        using NumBits  = int;
        using NumBytes = int;

        explicit IntegerLimits(NumBits numBits);

        [[nodiscard]]
        NumBytes GetNumBytes() const noexcept;

        [[nodiscard]]
        NumBits GetNumBits() const noexcept;

    private:
        NumBits m_NumBits;
    };

    [[nodiscard]]
    // not consteval because this is architecture dependent, and we might load that in at runtime later
    IntegerLimits GetIntegerLimits(StandardIntegerType type);

    using ConversionRank = int;

    [[nodiscard]]
    consteval ConversionRank GetStdIntConversionRank(StandardIntegerType type) {
        switch (type) {
            case StandardIntegerType::Char:
                return 1;
            case StandardIntegerType::Short:
                return 2;
            case StandardIntegerType::Int:
                return 3;
            case StandardIntegerType::Long:
                return 4;
            case StandardIntegerType::LongLong:
                return 5;
            default:
                return 0;
        }
    }

    //
    // [[nodiscard]]
    // consteval ConversionRank GetBitIntConversionRank(BitInteger const &type) {
    // }
    //
    // [[nodiscard]]
    // consteval ConversionRank GetConversionRank(IntegerType type) {
    //     if (std::holds_alternative<StandardIntegerType>(type)) {
    //         return GetStdIntConversionRank(std::get<StandardIntegerType>(type));
    //     }
    // }
}// namespace jcc::parsing_sema::types

#endif//TYPE_H
