#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "parser.h"

namespace jcc::preprocessor {
    class PreprocessorIterator;
}

namespace jcc::parsing_sema {
    [[nodiscard]]
    AstExpressionPtr ParseExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    );
}// namespace jcc::parsing_sema

#endif//EXPRESSION_H
