#ifndef PRINTER_H
#define PRINTER_H

#include <cstdint>
#include <string>

struct CompareFlags;

namespace jcc::codegen::printer {
    namespace load {
        [[nodiscard]]
        std::string LdaImmediate(std::int8_t value, CompareFlags &flags);

        [[nodiscard]]
        std::string LdaZeroPage(std::uint8_t address, CompareFlags &flags);

        [[nodiscard]]
        std::string LdxImmediate(std::int8_t value, CompareFlags &flags);

        [[nodiscard]]
        std::string LdxZeroPage(std::uint8_t address, CompareFlags &flags);

        [[nodiscard]]
        std::string LdyImmediate(std::int8_t value, CompareFlags &flags);

        [[nodiscard]]
        std::string LdyZeroPage(std::uint8_t address, CompareFlags &flags);
    }// namespace load

    namespace store {
        [[nodiscard]]
        std::string StaZeroPage(std::uint8_t address, CompareFlags &flags);

        [[nodiscard]]
        std::string
        StaZeroPageIndexedX(std::uint8_t address, CompareFlags &flags);

        [[nodiscard]]
        std::string StxZeroPage(std::uint8_t address, CompareFlags &flags);

        [[nodiscard]]
        std::string StyZeroPage(std::uint8_t address, CompareFlags &flags);
    }// namespace store

    namespace bitwise {
        [[nodiscard]]
        std::string AndImmediate(std::int8_t value, CompareFlags &flags);

        [[nodiscard]]
        std::string AndZeroPage(std::uint8_t address, CompareFlags &flags);
    }// namespace bitwise

    namespace arithmetic {
        [[nodiscard]]
        std::string
        IncrementZeroPage(std::uint8_t address, CompareFlags &flags);
    }
}// namespace jcc::codegen::printer

#endif//PRINTER_H
