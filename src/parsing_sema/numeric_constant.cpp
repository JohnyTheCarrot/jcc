#include "numeric_constant.h"

#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <string_view>

#include "misc/Diagnosis.h"
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

    llvm::Value *AstIntegerConstant::Codegen() {
        auto *type{m_Type.GetLLVMType()};

        return llvm::ConstantInt::get(type, m_Value, m_Type.IsSigned());
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
                    types::BitInteger{numMinBits + isForcedUnsigned}, signedness
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

    [[nodiscard]]
    bool IsFloatingPoint(char c) noexcept {
        switch (std::tolower(c)) {
            case '.':
            case 'e':
            case 'p':
                return true;
            default:
                return false;
        }
    }

    std::unique_ptr<AstIntegerConstant>
    ParseNumericConstant(tokenizer::Token &token) {
        auto [numberStr]{std::get<tokenizer::PpNumber>(token.m_Value)};

        constexpr auto RADIX_HEX{16};
        constexpr auto RADIX_OCT{8};
        constexpr auto RADIX_DEC{10};

        auto const numEnd{std::ranges::find_if(numberStr, IsFloatingPoint)};
        auto const isFloat{
                numEnd != numberStr.end() && IsFloatingPoint(*numEnd)
        };

        auto const [radix, numStart]{[&] {
            if (numberStr.starts_with("0x")) {
                return std::make_pair(RADIX_HEX, numberStr.cbegin() + 2);
            }

            if (!isFloat && numberStr.starts_with('0')) {
                return std::make_pair(RADIX_OCT, numberStr.cbegin() + 1);
            }

            return std::make_pair(RADIX_DEC, numberStr.cbegin());
        }()};

        if (isFloat) {
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(token.m_Span)
            };
        }
        auto const startOffset{std::distance(numberStr.cbegin(), numStart)};

        // TODO: this likely allows for separators in the middle of suffixes
        std::string numberWithoutSeparators;
        numberWithoutSeparators.reserve(numberStr.size() - startOffset);
        std::copy_if(
                numberStr.cbegin() + startOffset, numberStr.cend(),
                std::back_inserter(numberWithoutSeparators),
                [](char c) { return c != '\''; }
        );

        IntValue   integerValue{};
        auto const result{std::from_chars(
                numberWithoutSeparators.data(),
                numberWithoutSeparators.data() + numberWithoutSeparators.size(),
                integerValue, radix
        )};
        if (result.ec != std::errc{}) {
            throw FatalCompilerError{
                    Diagnosis::Kind::PRS_InvalidIntegerLiteral,
                    Span{token.m_Span}
            };
        }

        auto const minBits{CalcMinNumBits(integerValue)};
        // Use left-over suffix in result.ptr
        std::string_view const suffixStart{result.ptr};
        auto const             suffix{ParseIntegerSuffix(
                token.m_Span, suffixStart, minBits, radix == RADIX_DEC
        )};

        return std::make_unique<AstIntegerConstant>(suffix, integerValue);
    }// namespace internal
}// namespace jcc::parsing_sema
