#include "expression.h"

#include "additive_expression.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing_sema {
    AstExpressionPtr ParseExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        return ParseAdditiveExpression(current, end);
    }
}// namespace jcc::parsing_sema
