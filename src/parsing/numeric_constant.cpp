#include "numeric_constant.h"

#include <algorithm>
#include <array>
#include <assert.h>
#include <cctype>
#include <charconv>
#include <cmath>
#include <diagnostics/variants/parsing/invalid_floating_point_literal.hpp>
#include <diagnostics/variants/parsing/invalid_integer_literal.hpp>
#include <diagnostics/variants/parsing/unrecognized_floating_suffix.hpp>
#include <diagnostics/variants/parsing/unrecognized_integer_suffix.hpp>
#include <diagnostics/variants/sema/no_compat_int_type.hpp>
#include <functional>
#include <iostream>
#include <iterator>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>

#include "diagnostics/variants/todo.hpp"
#include "misc/Span.h"
#include "mjolnir/span.hpp"
#include "parsing/parser.h"
#include "parsing/types/type.h"
#include "tokenizer/token.h"

namespace jcc::parsing {
    AstIntegerConstant::AstIntegerConstant(
            Span span, types::IntegerType type, IntValue value
    )
        : AstExpression{std::move(span)}
        , m_Value{value}
        , m_Type{type} {
    }

    IntValue AstIntegerConstant::GetValue() const noexcept {
        return m_Value;
    }

    void AstIntegerConstant::AcceptOnExpression(ExpressionVisitor *visitor
    ) const {
        return visitor->Visit(this);
    }

    bool AstIntegerConstant::operator==(AstIntegerConstant const &other) const {
        return m_Value == other.m_Value &&
               GetNumericType() == other.GetNumericType();
    }

    void PrintTo(AstIntegerConstant const &astIntConst, std::ostream *os) {
        *os << "AstIntegerConstant{";
        PrintTo(astIntConst.GetNumericType(), os);
        *os << ", " << astIntConst.GetValue() << '}';
    }

    int CalcMinNumBits(std::int64_t value) noexcept {
        auto const floatingPointValue{static_cast<double>(value)};
        assert(floatingPointValue >= 0.);

        auto const log2Value{
                floatingPointValue == 0 ? 0.0 : std::log2(floatingPointValue)
        };

        return static_cast<int>(std::floor(log2Value) + 1);
    }

    enum class SignPossibilities { Signed = 1 << 0, Unsigned = 1 << 1 };

    /**
     * Types must be ordered from smallest to largest.
     *
     * This function will return the smallest compatible type that can hold a value of at least `numBits` bits.
     *
     * @param intSpan The span of the integer literal
     * @param types The list of types to choose from
     * @param numBits The minimum number of bits the type must be able to hold
     * @param signPossibilities The possible signs of the integer
     */
    [[nodiscard]]
    types::IntegerType ChooseSmallestCompatible(
            Span &intSpan, std::span<types::StandardIntegerType const> types,
            unsigned numBits, SignPossibilities signPossibilities
    ) {
        using namespace magic_enum::bitwise_operators;
        using Signedness = types::IntegerType::Signedness;

        for (auto const type : types) {
            auto const typeSize{
                    types::IntegerType::GetLLVMType(type)->getIntegerBitWidth()
            };
            auto const unsignedPossible{
                    (signPossibilities & SignPossibilities::Unsigned) ==
                    SignPossibilities::Unsigned
            };
            auto const signedPossible{
                    (signPossibilities & SignPossibilities::Signed) ==
                    SignPossibilities::Signed
            };

            if (signedPossible && typeSize - 1 /* sign bit */ >= numBits) {
                return types::IntegerType{type, Signedness::Signed};
            }

            if (unsignedPossible && typeSize >= numBits) {
                return types::IntegerType{type, Signedness::Unsigned};
            }
        }

        CompilerState::GetInstance()
                .EmplaceFatalDiagnostic<diagnostics::NoCompatIntType>(
                        std::move(intSpan.m_Source), intSpan.m_Span
                );
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
                    types::BitInteger{numMinBits + !isForcedUnsigned},
                    signedness
            };
        }

        CompilerState::GetInstance()
                .EmplaceTemporarilyFatalDiagnostic<
                        diagnostics::UnrecognizedIntegerSuffix>(
                        suffixSpan.m_Source, suffixSpan.m_Span
                );
    }

    types::IntegerType ParseIntegerSuffix(
            Span &completeSpan, std::string_view string_view,
            int numMinBitsNoSign, bool isDecimal
    ) {
        Span suffixSpan{
                completeSpan.m_Source,
                {completeSpan.m_Span.end() - string_view.size() - 1,
                 completeSpan.m_Span.end()}
        };

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

    AstFloatingConstant::AstFloatingConstant(
            Span span, types::FloatingType type, FloatingValue value
    )
        : AstExpression{std::move(span)}
        , m_Value{value}
        , m_Type{type} {
    }

    FloatingValue AstFloatingConstant::GetValue() const noexcept {
        return m_Value;
    }

    void AstFloatingConstant::AcceptOnExpression(ExpressionVisitor *visitor
    ) const {
        return visitor->Visit(this);
    }

    constexpr auto RADIX_HEX{16};
    constexpr auto RADIX_OCT{8};
    constexpr auto RADIX_DEC{10};

    [[nodiscard]]
    types::FloatingType
    ParseFloatingSuffix(Span &suffixSpan, std::string_view suffix) {
        if (suffix.empty()) {
            return types::FloatingType{types::StandardFloatingType::Double};
        }

        if (suffix == "f" || suffix == "F") {
            return types::FloatingType{types::StandardFloatingType::Float};
        }

        if (suffix == "l" || suffix == "L") {
            return types::FloatingType{types::StandardFloatingType::LongDouble};
        }

        CompilerState::GetInstance()
                .EmplaceTemporarilyFatalDiagnostic<
                        diagnostics::UnrecognizedFloatingSuffix>(
                        suffixSpan.m_Source, suffixSpan.m_Span
                );
    }

    [[nodiscard]]
    types::FloatingType ParseFloatContinued(
            tokenizer::Token &token, int radix, std::string_view const rest,
            int &exponent
    ) {
        if (radix == RADIX_HEX) {
            // TODO: P-notation
            CompilerState::GetInstance()
                    .EmplaceTemporarilyFatalDiagnostic<diagnostics::TodoError>(
                            std::move(token.m_Span.m_Source),
                            token.m_Span.m_Span
                    );
        }

        // At this point, the next character must be either 'e' or 'E', or it must be the start of a floating-suffix.
        if (rest.starts_with("e") || rest.starts_with("E")) {
            std::string_view const exponentStart{[&] {
                std::string_view strView{std::next(rest.cbegin()), rest.cend()};
                if (strView.starts_with('+'))
                    strView.remove_prefix(1);

                return strView;
            }()};

            auto const exponentResult{std::from_chars(
                    exponentStart.data(),
                    exponentStart.data() + exponentStart.size(), exponent
            )};
            if (exponentResult.ec != std::errc{}) {
                CompilerState::GetInstance()
                        .EmplaceTemporarilyFatalDiagnostic<
                                diagnostics::InvalidFloatingPointLiteral>(
                                std::move(token.m_Span.m_Source),
                                token.m_Span.m_Span
                        );
            }

            std::string_view const suffix{exponentResult.ptr, rest.end()};
            return ParseFloatingSuffix(token.m_Span, suffix);
        }

        return ParseFloatingSuffix(token.m_Span, rest);
    }

    AstExpressionPtr ParseFloatingConstant(
            tokenizer::Token &token, std::string_view numberWithoutSeparators,
            int radix
    ) {
        if (radix == RADIX_OCT) {
            // Leading zeros are allowed in octal literals, but they have no effect.
            radix = RADIX_DEC;
        }

        auto const             fractionalConstantEnd{std::ranges::find_if(
                numberWithoutSeparators,
                [&](char c) { return c == 'e' || c == 'E'; }
        )};
        std::string_view const fractionalConstant{
                numberWithoutSeparators.cbegin(), fractionalConstantEnd
        };
        double     fractionalValue{};
        auto const result{std::from_chars(
                fractionalConstant.data(),
                fractionalConstant.data() + fractionalConstant.size(),
                fractionalValue
        )};
        if (result.ec != std::errc{}) {
            CompilerState::GetInstance()
                    .EmplaceTemporarilyFatalDiagnostic<
                            diagnostics::InvalidFloatingPointLiteral>(
                            std::move(token.m_Span.m_Source),
                            token.m_Span.m_Span
                    );
        }

        // Floating-point literals without a fractional part must have an exponent.
        // But we don't need to check for that here because such a literal would have been lexed as an integer literal.

        types::FloatingType type{types::StandardFloatingType::Double};
        int                 exponent{0};
        if (fractionalConstantEnd != numberWithoutSeparators.cend()) {
            std::string_view const rest{
                    fractionalConstantEnd, numberWithoutSeparators.cend()
            };

            type = ParseFloatContinued(token, radix, rest, exponent);
        }
        if (result.ptr != fractionalConstantEnd) {
            std::string_view const suffix{
                    result.ptr, numberWithoutSeparators.cend()
            };
            type = ParseFloatingSuffix(token.m_Span, suffix);
        }

        fractionalValue *= std::pow(10., exponent);

        return std::make_unique<AstFloatingConstant>(
                std::move(token.m_Span), type, fractionalValue
        );
    }

    AstExpressionPtr ParseNumericConstant(tokenizer::Token &token) {
        auto [numberStr]{std::get<tokenizer::PpNumber>(token.m_Value)};

        auto const [radix, numStart]{[&] {
            if (numberStr.starts_with("0x")) {
                return std::make_pair(RADIX_HEX, numberStr.cbegin() + 2);
            }

            if (numberStr.starts_with('0')) {
                return std::make_pair(RADIX_OCT, numberStr.cbegin());
            }

            return std::make_pair(RADIX_DEC, numberStr.cbegin());
        }()};

        auto const startOffset{std::distance(numberStr.cbegin(), numStart)};

        // This does not allow for separators in the middle of suffixes, because we already checked for that in the tokenizer.
        std::string numberWithoutSeparators;
        numberWithoutSeparators.reserve(numberStr.size() - startOffset);
        std::copy_if(
                numberStr.cbegin() + startOffset, numberStr.cend(),
                std::back_inserter(numberWithoutSeparators),
                [](char c) { return c != '\''; }
        );

        auto const numEnd{
                std::ranges::find_if(numberWithoutSeparators, IsFloatingPoint)
        };
        auto const isFloat{
                numEnd != numberWithoutSeparators.end() &&
                IsFloatingPoint(*numEnd)
        };

        if (isFloat) {
            return ParseFloatingConstant(token, numberWithoutSeparators, radix);
        }

        IntValue   integerValue{};
        auto const result{std::from_chars(
                numberWithoutSeparators.data(),
                numberWithoutSeparators.data() + numberWithoutSeparators.size(),
                integerValue, radix
        )};
        if (result.ec != std::errc{}) {
            CompilerState::GetInstance()
                    .EmplaceTemporarilyFatalDiagnostic<
                            diagnostics::InvalidIntegerLiteral>(
                            std::move(token.m_Span.m_Source),
                            token.m_Span.m_Span
                    );
        }

        auto const minBits{CalcMinNumBits(integerValue)};
        // Use left-over suffix in result.ptr
        std::string_view const suffixStart{result.ptr};
        auto const             suffix{ParseIntegerSuffix(
                token.m_Span, suffixStart, minBits, radix == RADIX_DEC
        )};

        return std::make_unique<AstIntegerConstant>(
                std::move(token.m_Span), suffix, integerValue
        );
    }// namespace internal
}// namespace jcc::parsing
