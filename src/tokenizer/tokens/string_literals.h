#ifndef STRING_LITERALS_H
#define STRING_LITERALS_H

#include "tokenizer/token.h"// for ConstantPrefix (ptr only), Token

namespace jcc::tokenizer {
    class CharIter;
}

namespace jcc::tokenizer::string_literals {
    [[nodiscard]]
    // The caller is expected to have already consumed the opening double quote.
    Token Tokenize(
            CharIter &charIter, ConstantPrefix prefix,
            std::size_t startPos
    );
}

#endif//STRING_LITERALS_H
