#ifndef COMMON_H
#define COMMON_H
#include <ostream>
#include <variant>

namespace jcc::codegen {
    struct InstructionEnd final {};

    using BinaryOutput       = std::variant<std::byte, InstructionEnd>;
    using BinaryOutputStream = std::basic_ostream<char>;
}// namespace jcc::codegen

#endif//COMMON_H
