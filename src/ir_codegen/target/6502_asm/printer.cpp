#include "printer.h"

#include <format>

#include "ir_codegen/target/target.h"

namespace jcc::codegen::printer {
    namespace load {
        std::string LdaImmediate(std::int8_t value, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("lda #${:02X}\n", value);
        }

        std::string LdaZeroPage(std::uint8_t address, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("lda ${:02X}\n", address);
        }

        std::string LdxImmediate(std::int8_t value, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("ldx #${:02X}\n", value);
        }

        std::string LdxZeroPage(std::uint8_t address, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("ldx ${:02X}\n", address);
        }

        std::string LdyImmediate(std::int8_t value, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("ldy #${:02X}\n", value);
        }

        std::string LdyZeroPage(std::uint8_t address, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("ldy ${:02X}\n", address);
        }
    }// namespace load

    namespace store {
        std::string StaZeroPage(std::uint8_t address, CompareFlags &) {
            return std::format("sta ${:02X}\n", address);
        }

        std::string StaZeroPageIndexedX(std::uint8_t address, CompareFlags &) {
            return std::format("sta ${:02X}, x\n", address);
        }

        std::string StxZeroPage(std::uint8_t address, CompareFlags &) {
            return std::format("stx ${:02X}\n", address);
        }

        std::string StyZeroPage(std::uint8_t address, CompareFlags &) {
            return std::format("sty ${:02X}\n", address);
        }
    }// namespace store

    namespace bitwise {
        std::string AndImmediate(std::int8_t value, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("and #${:02X}\n", value);
        }

        std::string AndZeroPage(std::uint8_t address, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("and ${:02X}\n", address);
        }
    }// namespace bitwise

    namespace arithmetic {
        std::string
        IncrementZeroPage(std::uint8_t address, CompareFlags &flags) {
            flags |= CompareFlags{.zero = true, .negative = true};

            return std::format("inc ${:02X}\n", address);
        }
    }// namespace arithmetic
}// namespace jcc::codegen::printer
