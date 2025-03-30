#ifndef PRIMARY_EXPRESSION_H
#define PRIMARY_EXPRESSION_H

#include "ast_node.h"
#include "parser.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing {
    [[nodiscard]]
    AstExpressionPtr ParsePrimaryExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//PRIMARY_EXPRESSION_H
