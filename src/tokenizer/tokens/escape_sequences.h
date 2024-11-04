#ifndef ESCAPE_SEQUENCES_H
#define ESCAPE_SEQUENCES_H

#include <misc/compiler_data_types.h>// for Char32

namespace jcc {
    namespace tokenizer {
        class CharIter;
        enum class ConstantPrefix;
    }// namespace tokenizer
    struct SpanMarker;
}// namespace jcc

namespace jcc::tokenizer::escape_sequences {
    [[nodiscard]]
    // The caller is expected to have already consumed the backslash.
    compiler_data_types::Char32::type Tokenize(
            CharIter &charIter, SpanMarker const &backslashMarker,
            ConstantPrefix prefix
    );
}// namespace jcc::tokenizer::escape_sequences

#endif//ESCAPE_SEQUENCES_H
