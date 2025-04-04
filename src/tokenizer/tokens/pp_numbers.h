#ifndef PP_NUMBER_H
#define PP_NUMBER_H

#include <cstddef>// for size_t

#include "tokenizer/token.h"// for Token

namespace jcc::tokenizer {
    class CharIter;
}

namespace jcc::tokenizer::pp_numbers {
    [[nodiscard]]
    Token Tokenize(CharIter &charIter, std::size_t start, bool startsWithDot);
}

#endif//PP_NUMBER_H
