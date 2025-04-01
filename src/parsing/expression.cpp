#include "expression.h"

#include "equality_expression.hpp"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing {
    AstExpressionPtr ParseExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        return ParseEqualityExpression(current, end);
    }
}// namespace jcc::parsing
