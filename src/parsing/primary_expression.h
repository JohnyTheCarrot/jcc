#ifndef PRIMARY_EXPRESSION_H
#define PRIMARY_EXPRESSION_H

#include "ast_node.h"// for AstExpressionPtr

namespace jcc {
    namespace preprocessor {
        class PreprocessorIterator;
    }// namespace preprocessor
}// namespace jcc

namespace jcc::parsing {
    [[nodiscard]]
    AstExpressionPtr ParsePrimaryExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing

#endif//PRIMARY_EXPRESSION_H
