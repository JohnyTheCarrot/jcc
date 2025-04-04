#ifndef CHARACTER_CONSTANTS_H
#define CHARACTER_CONSTANTS_H

#include <cstddef>// for size_t

#include "tokenizer/token.h"// for ConstantPrefix (ptr only), Token

namespace jcc::tokenizer {
    class CharIter;
}

namespace jcc::tokenizer::character_constants {
    [[nodiscard]]
    // The caller is expected to have already consumed the opening single quote.
    Token
    Tokenize(CharIter &charIter, ConstantPrefix prefix, std::size_t startPos);
}

#endif//CHARACTER_CONSTANTS_H
