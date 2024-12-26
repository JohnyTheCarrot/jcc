#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include "parsing_sema/integer_constant.h"

using namespace jcc::parsing_sema;

using Iterator       = std::vector<jcc::tokenizer::Token>::const_iterator;
using IntConstParser = IntegerConstantParser<Iterator>;

using Signedness = types::IntegerType::Signedness;

class IntegerConstantParserSuffixTest
    : public testing::TestWithParam<std::tuple<std::string, IntegerSuffix>> {};

TEST_P(IntegerConstantParserSuffixTest, Suffixes) {
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

    IntConstParser parser{tokens.cbegin(), tokens.cend()};
    auto           suffixStartIt{input.cbegin()};
    auto const     suffix{
            parser.ParseIntegerSuffix(span, suffixStartIt, input.cend())
    };

    EXPECT_EQ(suffix, expected);
}

INSTANTIATE_TEST_SUITE_P(
        Suffixes, IntegerConstantParserSuffixTest,
        testing::Values(
                std::make_tuple(
                        "l",
                        IntegerSuffix{
                                Signedness::Unspecified,
                                types::StandardIntegerType::Long
                        }
                ),
                std::make_tuple(
                        "ll",
                        IntegerSuffix{
                                Signedness::Unspecified,
                                types::StandardIntegerType::LongLong
                        }
                ),
                std::make_tuple(
                        "u", IntegerSuffix{Signedness::Unsigned, std::nullopt}
                ),
                std::make_tuple(
                        "ul",
                        IntegerSuffix{
                                Signedness::Unsigned,
                                types::StandardIntegerType::Long
                        }
                ),
                std::make_tuple(
                        "ull",
                        IntegerSuffix{
                                Signedness::Unsigned,
                                types::StandardIntegerType::LongLong
                        }
                ),
                std::make_tuple(
                        "lu",
                        IntegerSuffix{
                                Signedness::Unsigned,
                                types::StandardIntegerType::Long
                        }
                ),
                std::make_tuple(
                        "llu",
                        IntegerSuffix{
                                Signedness::Unsigned,
                                types::StandardIntegerType::LongLong
                        }
                )
        )
);
