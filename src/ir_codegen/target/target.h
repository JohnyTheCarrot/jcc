#ifndef TARGET_H
#define TARGET_H

#include <cstdint>
#include <optional>
#include <ostream>

#include "common.h"
#include "register.h"

namespace jcc::codegen {
    // Used to check if an instruction set result flags so we can omit comparison instructions
    // These denote the flags that can be set by an instruction, not whether they are set
    struct CompareFlags final {
        bool zero : 1 {false};
        bool sign : 1 {false};
        bool carry : 1 {false};
        bool overflow : 1 {false};
        bool negative : 1 {false};

        [[nodiscard]]
        CompareFlags
        operator|(CompareFlags const &rhs) const;

        CompareFlags &operator|=(CompareFlags const &rhs);
    };

    struct InstructionResultInfo final {
        std::optional<Register> reg;
        CompareFlags            flags;

        [[nodiscard]]
        Register const *
        operator->() const;

        [[nodiscard]]
        Register *
        operator->();
    };

    class Target {
    public:
        virtual ~Target() = default;

        [[nodiscard]]
        virtual InstructionResultInfo
        EmitStoreInt8Immediate(std::int8_t value, BinaryOutputStream &out) = 0;

        [[nodiscard]]
        virtual InstructionResultInfo
        EmitInt8Copy(Register const &source, BinaryOutputStream &out) = 0;

        [[nodiscard]]
        virtual InstructionResultInfo EmitLogicalAndInt8Immediate(
                Register const &lhs, std::int8_t rhs, BinaryOutputStream &out
        ) = 0;

        [[nodiscard]]
        virtual InstructionResultInfo EmitLogicalAndInt8Register(
                Register const &lhs, Register const &rhs,
                BinaryOutputStream &out
        ) = 0;

        [[nodiscard]]
        virtual InstructionResultInfo
        EmitIncrement(Register const &reg, BinaryOutputStream &out) = 0;

        [[nodiscard]]
        virtual Register RegisterAllocate(registers::Size size) = 0;

        virtual void RegisterFree(registers::Id reg) = 0;
    };
}// namespace jcc::codegen

#endif//TARGET_H
