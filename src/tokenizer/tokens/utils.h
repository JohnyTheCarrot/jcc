#ifndef UTILS_H
#define UTILS_H

#include <cstddef> // for size_t
#include <optional>// for optional

#include "misc/compiler_data_types.h"// for Char32

namespace jcc::tokenizer {
    enum class ConstantPrefix;
    class CharIter;
}// namespace jcc::tokenizer

namespace jcc::tokenizer::utils {
    enum class ConstantType {
        String,
        Character,
    };

    // for character constants and string literals
    [[nodiscard]]
    std::optional<compiler_data_types::Char32::type> ReadSingleCharacter(
            CharIter &charIter, ConstantPrefix prefix, std::size_t startPos,
            ConstantType constantType
    );

    [[nodiscard]]
    bool IsNonDigit(char c);
}// namespace jcc::tokenizer::utils

#endif//UTILS_H
