#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include "parsing_sema/numeric_constant.h"

using namespace jcc::parsing_sema;

using Iterator       = std::vector<jcc::tokenizer::Token>::const_iterator;
using NumConstParser = NumericConstantParser<Iterator>;

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

    std::vector<jcc::tokenizer::Token> tokens;
    tokens.emplace_back(
            jcc::tokenizer::Token{jcc::tokenizer::PpNumber{input}, span}
    );

    NumConstParser parser{tokens.cbegin(), tokens.cend()};
    auto const     suffix{parser.ParseNumericConstant()};

    ASSERT_NE(suffix, nullptr);
    auto const astIntConst{dynamic_cast<AstIntegerConstant *>(suffix.get())};
    ASSERT_NE(astIntConst, nullptr);

    EXPECT_EQ(*astIntConst, expected);
}

// NOTE: Please make sure the `num` is a valid integer literal, NOT an expression.
#define ICP_RULE(num, type, sign)                                              \
    std::make_tuple(                                                           \
            #num, AstIntegerConstant{types::IntegerType{type, sign}, num}      \
    )

INSTANTIATE_TEST_SUITE_P(
        Suffixes, IntegerConstantParserTest,
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
                        32768 /* INT_MAX + 1 */,
                        types::StandardIntegerType::Long, Signedness::Signed
                ),
                // Non-decimal INT_MAX + 1 is an unsigned int, as non-decimal literals may be unsigned if it can't fit in the signed type.
                ICP_RULE(
                        0x8000, types::StandardIntegerType::Int,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        65536 /* UINT_MAX + 1 */,
                        types::StandardIntegerType::Long, Signedness::Signed
                ),
                ICP_RULE(
                        0x8000ull, types::StandardIntegerType::LongLong,
                        Signedness::Unsigned
                ),
                ICP_RULE(
                        2147483648 /* LONG_MAX + 1*/,
                        types::StandardIntegerType::LongLong, Signedness::Signed
                ),
                ICP_RULE(
                        0x80000000 /* LONG_MAX + 1*/,
                        types::StandardIntegerType::Long, Signedness::Unsigned
                ),
                ICP_RULE(
                        4294967295 /* ULONG_MAX + 1*/,
                        types::StandardIntegerType::LongLong, Signedness::Signed
                ),
                ICP_RULE(
                        0x100000000 /* ULONG_MAX + 1 */,
                        types::StandardIntegerType::LongLong, Signedness::Signed
                ),
                ICP_RULE(
                        1'000, types::StandardIntegerType::Int,
                        Signedness::Signed
                )
        )
);
