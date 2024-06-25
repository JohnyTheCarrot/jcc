#include "tokenizer/char_iter.h"
#include "misc/Span.h"
#include <gtest/gtest.h>

using namespace jcc::tokenizer;

using CharInfoList = std::vector<CharInfo>;

class CharIterTest : public testing::TestWithParam<std::tuple<std::string, CharInfoList>> {};

TEST_P(CharIterTest, CharIter) {
	std::stringstream is{std::get<0>(GetParam())};

	std::vector const     expected{std::get<1>(GetParam())};
	std::vector<CharInfo> actual;

	std::copy(CharIter{is, "test.c"}, CharIter::end(), std::back_inserter(actual));

	EXPECT_EQ(expected, actual);
}

INSTANTIATE_TEST_SUITE_P(
        Define, CharIterTest,
        testing::Values(
                std::make_tuple(
                        "hello",
                        CharInfoList{
                                {jcc::SpanMarker{1, 1, 0}, 'h'},
                                {jcc::SpanMarker{1, 2, 1}, 'e'},
                                {jcc::SpanMarker{1, 3, 2}, 'l'},
                                {jcc::SpanMarker{1, 4, 3}, 'l'},
                                {jcc::SpanMarker{1, 5, 4}, 'o'}
                        }
                ),
                std::make_tuple(
                        "hello\nworld",
                        CharInfoList{
                                {jcc::SpanMarker{1, 1, 0}, 'h'},
                                {jcc::SpanMarker{1, 2, 1}, 'e'},
                                {jcc::SpanMarker{1, 3, 2}, 'l'},
                                {jcc::SpanMarker{1, 4, 3}, 'l'},
                                {jcc::SpanMarker{1, 5, 4}, 'o'},
                                {jcc::SpanMarker{1, 6, 5}, '\n'},
                                {jcc::SpanMarker{2, 1, 0}, 'w'},
                                {jcc::SpanMarker{2, 2, 1}, 'o'},
                                {jcc::SpanMarker{2, 3, 2}, 'r'},
                                {jcc::SpanMarker{2, 4, 3}, 'l'},
                                {jcc::SpanMarker{2, 5, 4}, 'd'}
                        }
                ),
                std::make_tuple(
                        "hello\r\nworld",
                        CharInfoList{
                                {jcc::SpanMarker{1, 1, 0}, 'h'},
                                {jcc::SpanMarker{1, 2, 1}, 'e'},
                                {jcc::SpanMarker{1, 3, 2}, 'l'},
                                {jcc::SpanMarker{1, 4, 3}, 'l'},
                                {jcc::SpanMarker{1, 5, 4}, 'o'},
                                {jcc::SpanMarker{1, 7, 6}, '\n'},
                                {jcc::SpanMarker{2, 1, 0}, 'w'},
                                {jcc::SpanMarker{2, 2, 1}, 'o'},
                                {jcc::SpanMarker{2, 3, 2}, 'r'},
                                {jcc::SpanMarker{2, 4, 3}, 'l'},
                                {jcc::SpanMarker{2, 5, 4}, 'd'}
                        }
                ),
                // escaped newline
                std::make_tuple(
                        "hello\\\nworld",
                        CharInfoList{
                                {jcc::SpanMarker{1, 1, 0}, 'h'},
                                {jcc::SpanMarker{1, 2, 1}, 'e'},
                                {jcc::SpanMarker{1, 3, 2}, 'l'},
                                {jcc::SpanMarker{1, 4, 3}, 'l'},
                                {jcc::SpanMarker{1, 5, 4}, 'o'},
                                {jcc::SpanMarker{2, 1, 0}, 'w'},
                                {jcc::SpanMarker{2, 2, 1}, 'o'},
                                {jcc::SpanMarker{2, 3, 2}, 'r'},
                                {jcc::SpanMarker{2, 4, 3}, 'l'},
                                {jcc::SpanMarker{2, 5, 4}, 'd'}
                        }
                ),
                // escaped newline with \r
                std::make_tuple(
                        "hello\\\r\nworld",
                        CharInfoList{
                                {jcc::SpanMarker{1, 1, 0}, 'h'},
                                {jcc::SpanMarker{1, 2, 1}, 'e'},
                                {jcc::SpanMarker{1, 3, 2}, 'l'},
                                {jcc::SpanMarker{1, 4, 3}, 'l'},
                                {jcc::SpanMarker{1, 5, 4}, 'o'},
                                {jcc::SpanMarker{2, 1, 0}, 'w'},
                                {jcc::SpanMarker{2, 2, 1}, 'o'},
                                {jcc::SpanMarker{2, 3, 2}, 'r'},
                                {jcc::SpanMarker{2, 4, 3}, 'l'},
                                {jcc::SpanMarker{2, 5, 4}, 'd'}
                        }
                )
        )
);
