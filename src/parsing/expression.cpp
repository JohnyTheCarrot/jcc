#include "expression.h"

#include "preprocessor/preprocessor_iterator.h"
#include "shift_expression.h"

namespace jcc::parsing {
    AstExpressionPtr ParseExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        return ParseShiftExpression(current, end);
    }
}// namespace jcc::parsing
