#include "platform.h"

#include <cassert>

namespace jcc::platform {
    ByteSize
    GetTypeSize(parsing_sema::types::StandardIntegerType type) noexcept {
        using StandardIntegerType = parsing_sema::types::StandardIntegerType;

        switch (type) {
            case StandardIntegerType::Char:
                return 1;
            case StandardIntegerType::Short:
            case StandardIntegerType::Int:
                return 2;
            case StandardIntegerType::Long:
                return 4;
            case StandardIntegerType::LongLong:
                return 8;
        }

        assert(false);
        return 0;
    }

    TypeMax GetTypeMax(parsing_sema::types::StandardIntegerType type) noexcept {
        auto const typeSize{GetTypeSize(type)};

        return (1ull << typeSize * 8) - 1;
    }
}// namespace jcc::platform
