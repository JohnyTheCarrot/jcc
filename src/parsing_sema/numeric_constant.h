#ifndef NUMERIC_CONSTANT_H
#define NUMERIC_CONSTANT_H

#include <functional>
#include <iostream>
#include <random>
#include <string_view>

#include "parser.h"
#include "types/type.h"

namespace jcc::parsing_sema {
    using IntValue      = std::int64_t;
    using NumConstValue = std::variant<IntValue, double>;

    class AstIntegerConstant final : public AstNode {
        types::IntegerType m_Type;
        IntValue           m_Value;

    public:
        AstIntegerConstant(types::IntegerType type, IntValue value) noexcept;

        [[nodiscard]]
        types::IntegerType GetType() const noexcept;

        [[nodiscard]]
        NumConstValue GetValue() const noexcept;

        [[nodiscard]]
        bool operator==(AstIntegerConstant const &other) const;
    };

    void PrintTo(AstIntegerConstant const &astIntConst, std::ostream *os);

    [[nodiscard]]
    int CalcMinNumBits(std::int64_t value) noexcept;

    [[nodiscard]]
    types::IntegerType ParseIntegerSuffix(
            Span &completeSpan, std::string_view string_view,
            int  numMinBitsNoSign,// Num of bits excluding sign bit
            bool isDecimal
    );

    template<typename TIterator>
        requires TokenIterator<TIterator>
    class NumericConstantParser final : public Parser<TIterator> {
        NumConstValue m_Value{};

    public:
        using Parser<TIterator>::Parser;

        typename NumericConstantParser::NodePtr ParseNumericConstant() {
            if (this->IsExhausted()) {
                return nullptr;
            }

            auto token{*this->GetCurrent()};

            tokenizer::PpNumber ppNumber{
                    std::get<tokenizer::PpNumber>(token.m_Value)
            };

            constexpr auto RADIX_HEX{16};
            constexpr auto RADIX_OCT{8};
            constexpr auto RADIX_DEC{10};

            auto const numEnd{std::ranges::find_if(
                    ppNumber.m_Number,
                    [](char c) {
                        switch (std::tolower(c)) {
                            // Floating point significant digits postfix
                            case '.':
                            case 'e':
                            case 'p':

                            // Floating point suffix
                            case 'f':

                            // Integer suffixes
                            case 'l':
                            case 'w':
                            case 'u':
                                return true;
                            default:
                                return false;
                        }
                    }
            )};
            auto const isFloat{
                    numEnd != ppNumber.m_Number.end() &&
                    (*numEnd == '.' || *numEnd == 'e' || *numEnd == 'p')
            };

            auto const [radix, numStart]{[&] {
                if (ppNumber.m_Number.starts_with("0x")) {
                    return std::make_pair(
                            RADIX_HEX, ppNumber.m_Number.cbegin() + 2
                    );
                }

                if (!isFloat && ppNumber.m_Number.starts_with('0')) {
                    return std::make_pair(
                            RADIX_OCT, ppNumber.m_Number.cbegin() + 1
                    );
                }

                return std::make_pair(RADIX_DEC, ppNumber.m_Number.cbegin());
            }()};

            if (isFloat) {
                double     floatingValue{};
                auto const result{std::from_chars(
                        ppNumber.m_Number.data(),
                        ppNumber.m_Number.data() + ppNumber.m_Number.size(),
                        floatingValue
                )};
                if (result.ec != std::errc{}) {
                    throw FatalCompilerError{
                            Diagnosis::Kind::PRS_InvalidFloatingPointLiteral,
                            Span{token.m_Span}
                    };
                }

                std::cout << floatingValue << std::endl;
                return nullptr;
            }

            auto const startOffset{
                    std::distance(ppNumber.m_Number.cbegin(), numStart)
            };
            IntValue   integerValue{};
            auto const result{std::from_chars(
                    ppNumber.m_Number.data() + startOffset,
                    ppNumber.m_Number.data() + startOffset +
                            (ppNumber.m_Number.size() - startOffset),
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
        }
    };
}// namespace jcc::parsing_sema

#endif//NUMERIC_CONSTANT_H
