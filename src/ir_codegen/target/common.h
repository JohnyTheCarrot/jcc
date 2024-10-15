#ifndef COMMON_H
#define COMMON_H
#include <ostream>
#include <variant>

namespace jcc::codegen {
    using BinaryOutput       = char;
    using BinaryOutputStream = std::basic_ostream<BinaryOutput>;
}// namespace jcc::codegen

#endif//COMMON_H
