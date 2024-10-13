// TODO: pImpl

#ifndef ASM6502_H
#define ASM6502_H
#include <array>
#include <list>
#include <optional>
#include <vector>

#include "ir_codegen/target/target.h"

namespace jcc::codegen::asm6502 {
    enum class Opcode {
        LDA,
        LDX,
        LDY,
        STA,
        STX,
        STY,
        TAX,
        TAY,
        TSX,
        TXA,
        TXS,
        TYA,
        ADC,
        AND,
        ASL,
        BIT,
        CMP,
        CPX,
        CPY,
        DEC,
        EOR,
        INC,
        LSR,
        ORA,
        ROL,
        ROR,
        SBC,
        JMP,
        JSR,
        RTS,
        BCC,
        BCS,
        BEQ,
        BMI,
        BNE,
        BPL,
        BVC,
        BVS,
        CLC,
        CLD,
        CLI,
        CLV,
        SEC,
        SED,
        SEI,
        BRK,
        NOP,
        RTI,
        PHA,
        PHP,
        PLA,
        PLP,
        DEY,
        INY,
        DEX,
        INX,
    };

    enum class AddressingMode {
        Immediate,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        Absolute,
        AbsoluteX,
        AbsoluteY,
        Indirect,
        IndirectX,
        IndirectY,
        Accumulator,
        Implied,
        Relative,
    };

    struct RegisterOccupation final {
        bool a : 1 {false};
        bool x : 1 {false};
        bool y : 1 {false};
    };

    using ZpRegister = std::size_t;

    class Target6502Asm;

    constexpr std::size_t c_ZeroPageSize = 256;

    // Registers need not necessarily be contiguous, so each register may have its own zeropage address
    class ZpRegister6502 final {
    public:
        using Indices = std::array<std::uint8_t, registers::c_MaxSize>;

        ZpRegister6502(
                Target6502Asm &target, registers::Size size,
                Indices const &indices
        );

        ~ZpRegister6502();

        ZpRegister6502(ZpRegister6502 const &) = delete;

        ZpRegister6502 &operator=(ZpRegister6502 const &) = delete;

        ZpRegister6502(ZpRegister6502 &&) noexcept;

        ZpRegister6502 &operator=(ZpRegister6502 &&) noexcept;

        [[nodiscard]]
        Indices const &GetIndices() const;

        [[nodiscard]]
        std::uint8_t GetIndex(std::size_t index) const;

        [[nodiscard]]
        registers::Size GetSize() const;

    private:
        Indices         m_Indices;
        Target6502Asm  *m_Target;
        registers::Size m_Size;

        friend class Target6502Asm;
        void Cleanup();
    };

    class Target6502Asm final : public Target {
        std::vector<ZpRegister>                    m_ZeroPageOccupied{};
        std::vector<std::optional<ZpRegister6502>> m_RegisterInfo{};
        RegisterOccupation                         m_RegistersOccupied;

        friend void ZpRegister6502::Cleanup();

    public:
        Target6502Asm();

        ~Target6502Asm() override;

        void FreeZeroPageRegister(ZpRegister value);

        Register RegisterAllocate(registers::Size size) override;

        void RegisterFree(registers::Id reg) override;

        InstructionResultInfo EmitStoreInt8Immediate(
                std::int8_t value, BinaryOutputStream &out
        ) override;

        [[nodiscard]]
        InstructionResultInfo
        EmitInt8Copy(Register const &source, BinaryOutputStream &out) override;

        [[nodiscard]]
        InstructionResultInfo EmitLogicalAndInt8Immediate(
                Register const &lhs, std::int8_t rhs, BinaryOutputStream &out
        ) override;

        InstructionResultInfo EmitLogicalAndInt8Register(
                Register const &lhs, Register const &rhs,
                BinaryOutputStream &out
        ) override;

        [[nodiscard]]
        InstructionResultInfo
        EmitIncrement(Register const &reg, BinaryOutputStream &out) override;
    };
}// namespace jcc::codegen::asm6502

#endif//ASM6502_H
