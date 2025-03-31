#include "expression.h"

#include "preprocessor/preprocessor_iterator.h"
#include "relational_expression.hpp"

namespace jcc::parsing {
    AstExpressionPtr ParseExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        return ParseRelationalExpression(current, end);
    }
}// namespace jcc::parsing
