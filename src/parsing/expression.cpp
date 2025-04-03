#include "expression.h"

#include "bitwise_xor_expression.hpp"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing {
    AstExpressionPtr ParseExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        return ParseBitwiseXorExpression(current, end);
    }
}// namespace jcc::parsing
