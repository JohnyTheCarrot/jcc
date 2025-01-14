#ifndef PLATFORM_H
#define PLATFORM_H

#include "parsing_sema/types/type.h"

namespace jcc::platform {
    using ByteSize = int;
    using TypeMax  = std::uint64_t;

    [[nodiscard]]
    ByteSize
    GetTypeSize(parsing_sema::types::StandardIntegerType type) noexcept;

    [[nodiscard]]
    TypeMax GetTypeMax(parsing_sema::types::StandardIntegerType type) noexcept;
}// namespace jcc::platform

#endif//PLATFORM_H
