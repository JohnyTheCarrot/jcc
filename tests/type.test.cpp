#include "parsing_sema/types/type.h"

#include <gtest/gtest.h>

using namespace jcc::parsing_sema;

using Signedness = types::IntegerType::Signedness;

class IntPromoTest
    : public testing::TestWithParam<
              std::tuple<types::IntegerType, types::IntegerType>> {};

TEST_P(IntPromoTest, IntPromotion) {
    auto const &[input, expected] = GetParam();

    EXPECT_EQ(input.Promote(), expected);
}

constexpr types::IntegerType c_SignedInt{
        types::StandardIntegerType::Int, Signedness::Signed
};

INSTANTIATE_TEST_SUITE_P(
        IntPromotions, IntPromoTest,
        testing::Values(
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Bool,
                                Signedness::Signed
                        },
                        c_SignedInt
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Char,
                                Signedness::Signed
                        },
                        c_SignedInt
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Char,
                                Signedness::Unsigned
                        },
                        c_SignedInt
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Short,
                                Signedness::Signed
                        },
                        c_SignedInt
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Int,
                                Signedness::Signed
                        },
                        c_SignedInt
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Long,
                                Signedness::Signed
                        },
                        types::IntegerType{
                                types::StandardIntegerType::Long,
                                Signedness::Signed
                        }
                )
        )
);

class UsualConversionTest
    : public testing::TestWithParam<std::tuple<
              types::IntegerType, types::IntegerType, types::IntegerType>> {};

TEST_P(UsualConversionTest, UsualArithmeticConversions) {
    auto const &[lhs, rhs, expected] = GetParam();

    auto const result{lhs.UsualArithmeticConversion(rhs)};

    EXPECT_EQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(
        UsualArithmeticConversionsTest, UsualConversionTest,
        testing::Values(
                std::make_tuple(c_SignedInt, c_SignedInt, c_SignedInt),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Int,
                                Signedness::Unsigned
                        },
                        c_SignedInt,
                        types::IntegerType{
                                types::StandardIntegerType::Int,
                                Signedness::Unsigned
                        }
                ),
                std::make_tuple(
                        c_SignedInt,
                        types::IntegerType{
                                types::StandardIntegerType::Long,
                                Signedness::Signed
                        },
                        types::IntegerType{
                                types::StandardIntegerType::Long,
                                Signedness::Signed
                        }
                ),
                std::make_tuple(
                        c_SignedInt,
                        types::IntegerType{
                                types::StandardIntegerType::Long,
                                Signedness::Unsigned
                        },
                        types::IntegerType{
                                types::StandardIntegerType::Long,
                                Signedness::Unsigned
                        }
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Short,
                                Signedness::Signed
                        },
                        c_SignedInt, c_SignedInt
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::Short,
                                Signedness::Unsigned
                        },
                        c_SignedInt, c_SignedInt
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::LongLong,
                                Signedness::Signed
                        },
                        c_SignedInt,
                        types::IntegerType{
                                types::StandardIntegerType::LongLong,
                                Signedness::Signed
                        }
                ),
                std::make_tuple(
                        types::IntegerType{
                                types::StandardIntegerType::LongLong,
                                Signedness::Unsigned
                        },
                        types::IntegerType{
                                types::StandardIntegerType::Long,
                                Signedness::Signed
                        },
                        types::IntegerType{
                                types::StandardIntegerType::LongLong,
                                Signedness::Unsigned
                        }
                )
        )
);
