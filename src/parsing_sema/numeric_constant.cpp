#include "numeric_constant.h"

#include <magic_enum/magic_enum.hpp>

#include "platform/platform.h"

namespace jcc::parsing_sema {
    AstIntegerConstant::AstIntegerConstant(
            types::IntegerType type, IntValue value
    ) noexcept
        : m_Type{type}
        , m_Value{value} {
    }

    types::IntegerType AstIntegerConstant::GetType() const noexcept {
        return m_Type;
    }

    NumConstValue AstIntegerConstant::GetValue() const noexcept {
        return m_Value;
    }

    bool AstIntegerConstant::operator==(AstIntegerConstant const &other) const {
        return m_Type == other.m_Type && m_Value == other.m_Value;
    }

    void PrintTo(AstIntegerConstant const &astIntConst, std::ostream *os) {
        *os << "AstIntegerConstant{";
        PrintTo(astIntConst.GetType(), os);
        *os << ", ";
        std::visit(
                [&os](auto const &value) { *os << value; },
                astIntConst.GetValue()
        );
        *os << '}';
    }

    int CalcMinNumBits(std::int64_t value) noexcept {
        auto const floatingPointValue{static_cast<double>(value)};
        assert(floatingPointValue >= 0.);

        auto const log2Value{std::log2(floatingPointValue)};

        return static_cast<int>(std::floor(log2Value) + 1);
    }

    enum class SignPossibilities { Signed = 1 << 0, Unsigned = 1 << 1 };

    /**
     * Types must be ordered from smallest to largest.
     */
    [[nodiscard]]
    types::IntegerType ChooseSmallestCompatible(
            Span &intSpan, std::span<types::StandardIntegerType const> types,
            int numBits, SignPossibilities signPossibilities
    ) {
        using namespace magic_enum::bitwise_operators;
        using Signedness = types::IntegerType::Signedness;

        for (auto const type : types) {
            auto const typeSize{platform::GetTypeSize(type)};
            auto const unsignedPossible{
                    (signPossibilities & SignPossibilities::Unsigned) ==
                    SignPossibilities::Unsigned
            };
            auto const signedPossible{
                    (signPossibilities & SignPossibilities::Signed) ==
                    SignPossibilities::Signed
            };

            if (signedPossible && typeSize * 8 - 1 /* sign bit */ >= numBits) {
                return types::IntegerType{type, Signedness::Signed};
            }

            if (unsignedPossible && typeSize * 8 >= numBits) {
                return types::IntegerType{type, Signedness::Unsigned};
            }
        }

        throw FatalCompilerError{
                Diagnosis::Kind::SEMA_NoCompatibleIntegerType,
                std::move(intSpan)
        };
    }

    [[nodiscard]]
    types::IntegerType ParseIntegerType(
            Span &intSpan, Span &suffixSpan, std::string_view typeSuffix,
            int numMinBits, bool isDecimal, bool isForcedUnsigned
    ) {
        auto signPossibilities{
                isForcedUnsigned ? SignPossibilities::Unsigned
                                 : SignPossibilities::Signed
        };

        if (!isDecimal) {
            using namespace magic_enum::bitwise_operators;

            // The standard dictates that for octal, hex, and binary literals the compatible type list includes the unsigned types.
            signPossibilities |= SignPossibilities::Unsigned;
        }

        if (typeSuffix.empty()) {
            constexpr std::array possibleTypes{
                    types::StandardIntegerType::Int,
                    types::StandardIntegerType::Long,
                    types::StandardIntegerType::LongLong
            };

            return ChooseSmallestCompatible(
                    intSpan, possibleTypes, numMinBits, signPossibilities
            );
        }

        if (typeSuffix == "l" || typeSuffix == "L") {
            constexpr std::array possibleTypes{
                    types::StandardIntegerType::Long,
                    types::StandardIntegerType::LongLong
            };

            return ChooseSmallestCompatible(
                    intSpan, possibleTypes, numMinBits, signPossibilities
            );
        }

        if (typeSuffix == "ll" || typeSuffix == "LL") {
            constexpr std::array possibleTypes{
                    types::StandardIntegerType::LongLong
            };

            return ChooseSmallestCompatible(
                    intSpan, possibleTypes, numMinBits, signPossibilities
            );
        }

        if (typeSuffix == "wb" || typeSuffix == "WB") {
            using Signedness = types::IntegerType::Signedness;
            auto const signedness{
                    isForcedUnsigned ? Signedness::Unsigned : Signedness::Signed
            };

            return types::IntegerType{
                    types::BitInteger{numMinBits}, signedness
            };
        }

        throw FatalCompilerError{
                Diagnosis::Kind::PRS_UnrecognizedIntegerSuffix,
                std::move(suffixSpan)
        };
    }

    types::IntegerType ParseIntegerSuffix(
            Span &completeSpan, std::string_view string_view,
            int numMinBitsNoSign, bool isDecimal
    ) {
        Span suffixSpan{completeSpan};
        suffixSpan.m_Start =
                completeSpan.m_End - (static_cast<int>(string_view.size()) - 1);

        if (string_view.starts_with('u') || string_view.starts_with('U')) {
            std::string_view const typeSuffix{
                    std::next(string_view.begin()), string_view.end()
            };

            return ParseIntegerType(
                    completeSpan, suffixSpan, typeSuffix, numMinBitsNoSign,
                    isDecimal, true
            );
        }

        auto const endsWithUnsigned{
                string_view.ends_with('u') || string_view.ends_with('U')
        };

        auto const typeSuffixEnd{
                endsWithUnsigned ? string_view.end() - 1 : string_view.end()
        };

        std::string_view const typeSuffix{string_view.begin(), typeSuffixEnd};

        return ParseIntegerType(
                completeSpan, suffixSpan, typeSuffix, numMinBitsNoSign,
                isDecimal, endsWithUnsigned
        );
    }
}// namespace jcc::parsing_sema
