#ifndef JCC_COMPILER_DATA_TYPES_H
#define JCC_COMPILER_DATA_TYPES_H

#include <cstdint>
#include <limits>
#include <type_traits>

// TODO: make this file read out from LLVM

namespace jcc::compiler_data_types {
    template<typename Type>
    struct TypeInfo final : std::numeric_limits<Type> {
        using type = Type;
        static constexpr std::size_t mask{
                std::numeric_limits<std::make_unsigned_t<Type>>::max()
        };
    };

    using IntLeast8_T  = TypeInfo<int8_t>;
    using UintLeast8_T = TypeInfo<uint8_t>;

    using IntLeast16_T  = TypeInfo<int16_t>;
    using UintLeast16_T = TypeInfo<uint16_t>;

    using IntLeast32_T  = TypeInfo<int32_t>;
    using UintLeast32_T = TypeInfo<uint32_t>;

    using IntLeast64_T  = TypeInfo<int64_t>;
    using UintLeast64_T = TypeInfo<uint64_t>;

    using Char   = TypeInfo<unsigned char>;
    using Char8  = UintLeast8_T;
    using WChar  = Char8;
    using Char16 = UintLeast16_T;// equal per the standard
    using Char32 = UintLeast32_T;// equal per the standard
    using Int    = TypeInfo<int32_t>;
}// namespace jcc::compiler_data_types

#endif//JCC_COMPILER_DATA_TYPES_H
