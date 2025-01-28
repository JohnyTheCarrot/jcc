#include <magic_enum/magic_enum.hpp>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include "parsing_sema/numeric_constant.h"
#include "parsing_sema/types.h"

using namespace jcc::parsing_sema;

using Iterator = std::vector<jcc::tokenizer::Token>::const_iterator;

using Signedness = types::IntegerType::Signedness;

class IntegerConstantParserTest
    : public testing::TestWithParam<
              std::tuple<std::string, AstIntegerConstant>> {};

TEST_P(IntegerConstantParserTest, Suffixes) {
    auto const &[input, expected] = GetParam();

    auto const         length{static_cast<int>(input.size())};
    std::istringstream iss{input};

    jcc::Span span{
            std::make_shared<std::string>("test"), jcc::SpanMarker{},
            jcc::SpanMarker{1, length, length - 1}, &iss
    };

    jcc::tokenizer::Token token{jcc::tokenizer::PpNumber{input}, span};
    auto const            suffix{ParseNumericConstant(token)};

    ASSERT_NE(suffix, nullptr);
    ASSERT_NE(suffix, nullptr);

    EXPECT_EQ(*suffix, expected);
}

jcc::Span const c_NullSpan{
        nullptr, jcc::SpanMarker{}, jcc::SpanMarker{}, nullptr
};

// NOTE: Please make sure the `num` is a valid integer literal, NOT an expression.
#define ICP_RULE(num, type, sign)                                              \
    std::make_tuple(                                                           \
            #num,                                                              \
            AstIntegerConstant{                                                \
                    c_NullSpan, types::IntegerType{type, sign}, num            \
            }                                                                  \
    )

INSTANTIATE_TEST_SUITE_P(
        IntegerConstants, IntegerConstantParserTest,
        testing::Values(
                ICP_RULE(
                        1, types::StandardIntegerType::Int, Signedness::Signed
                ),
                ICP_RULE(
                        1u, types::StandardIntegerType::Int,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        1l, types::StandardIntegerType::Long, Signedness::Signed
                ),
                ICP_RULE(
                        1ll, types::StandardIntegerType::LongLong,
                        Signedness::Signed
                ),
                ICP_RULE(
                        1ul, types::StandardIntegerType::Long,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        1ull, types::StandardIntegerType::LongLong,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        1lu, types::StandardIntegerType::Long,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        1llu, types::StandardIntegerType::LongLong,
                        Signedness::Unsigned
                ),
                // Decimal INT_MAX + 1 is a long, as it doesn't fit in an int.
                ICP_RULE(
                        2147483648 /* INT_MAX + 1 */,
                        types::StandardIntegerType::Long, Signedness::Signed
                ),
                // Non-decimal INT_MAX + 1 is an unsigned int, as non-decimal literals may be unsigned if it can't fit in the signed type.
                ICP_RULE(
                        0x80000000, types::StandardIntegerType::Int,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        0xFFFF'FFFF, types::StandardIntegerType::Int,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        4294967296 /* UINT_MAX + 1 */,
                        types::StandardIntegerType::Long, Signedness::Signed
                ),
                ICP_RULE(
                        0x8000ull, types::StandardIntegerType::LongLong,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        1'000, types::StandardIntegerType::Int,
                        Signedness::Signed
                ),
                std::make_tuple(
                        "7wb",
                        AstIntegerConstant{
                                c_NullSpan,
                                {types::BitInteger{4}, Signedness::Signed},
                                7
                        }
                ),
                std::make_tuple(
                        "7uwb",
                        AstIntegerConstant{
                                c_NullSpan,
                                {types::BitInteger{3}, Signedness::Unsigned},
                                7
                        }
                )
        )
);

class BasicTypeCompatibilityTest
    : public testing::TestWithParam<std::tuple<BasicTypeSpecifier, bool>> {};

TEST_P(BasicTypeCompatibilityTest, BasicTypeCompatibility) {
    auto const &[first, expected] = GetParam();

    EXPECT_EQ(AreBasicTypesCompatible(first), expected);
}

using namespace magic_enum::bitwise_operators;

INSTANTIATE_TEST_CASE_P(
        BasicTypeCompatibility, BasicTypeCompatibilityTest,
        testing::Values(
                std::make_tuple(BasicTypeSpecifier::Void, true),
                std::make_tuple(
                        BasicTypeSpecifier::Void | BasicTypeSpecifier::Int,
                        false
                ),
                std::make_tuple(
                        BasicTypeSpecifier::Char | BasicTypeSpecifier::Signed,
                        true
                ),
                std::make_tuple(
                        BasicTypeSpecifier::Char | BasicTypeSpecifier::Unsigned,
                        true
                ),
                std::make_tuple(
                        BasicTypeSpecifier::Char | BasicTypeSpecifier::Signed |
                                BasicTypeSpecifier::Unsigned,
                        false
                )
        )
);
