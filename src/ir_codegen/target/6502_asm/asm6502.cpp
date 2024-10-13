#include "asm6502.h"

#include <algorithm>
#include <format>
#include <iostream>

#include "printer.h"

namespace jcc::codegen::asm6502 {
    void ZpRegister6502::Cleanup() {
        if (m_Target == nullptr)
            return;

        std::cout << "Freeing register of size " << static_cast<int>(m_Size)
                  << '\n';
        auto const nbRegisterSize{static_cast<int>(m_Size)};

        for (int i = 0; i < nbRegisterSize; ++i) {
            auto const index{m_Indices[i]};
            m_Target->m_ZeroPageOccupied[index] = false;
        }
    }

    ZpRegister6502::ZpRegister6502(
            Target6502Asm &target, registers::Size size, Indices const &indices
    )
        : m_Indices{indices}
        , m_Target{&target}
        , m_Size{size} {
    }

    ZpRegister6502::~ZpRegister6502() {
        Cleanup();
    }

    ZpRegister6502::ZpRegister6502(ZpRegister6502 &&other) noexcept
        : m_Indices{other.m_Indices}
        , m_Target{other.m_Target}
        , m_Size{other.m_Size} {
        other.m_Target = nullptr;
    }

    ZpRegister6502 &ZpRegister6502::operator=(ZpRegister6502 &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        Cleanup();
        m_Indices = other.m_Indices;
        m_Target  = other.m_Target;
        m_Size    = other.m_Size;

        other.m_Target = nullptr;

        return *this;
    }

    ZpRegister6502::Indices const &ZpRegister6502::GetIndices() const {
        return m_Indices;
    }

    std::uint8_t ZpRegister6502::GetIndex(std::size_t index) const {
        return m_Indices[index];
    }

    registers::Size ZpRegister6502::GetSize() const {
        return m_Size;
    }

    Target6502Asm::Target6502Asm() {
        m_ZeroPageOccupied.resize(c_ZeroPageSize);
    }

    Target6502Asm::~Target6502Asm() = default;

    InstructionResultInfo Target6502Asm::EmitStoreInt8Immediate(
            std::int8_t value, BinaryOutputStream &out
    ) {
        Register     reg{RegisterAllocate(registers::Size::Byte)};
        CompareFlags flags{};
        std::uint8_t zeroPageReg{
                m_RegisterInfo.at(reg.GetId().value)->GetIndex(0)
        };

        if (!m_RegistersOccupied.a) {
            out << printer::load::LdaImmediate(value, flags)
                << printer::store::StaZeroPage(zeroPageReg, flags);
        } else if (!m_RegistersOccupied.x) {
            out << printer::load::LdxImmediate(value, flags)
                << printer::store::StxZeroPage(zeroPageReg, flags);
        } else if (!m_RegistersOccupied.y) {
            out << printer::load::LdyImmediate(value, flags)
                << printer::store::StyZeroPage(zeroPageReg, flags);
        } else {
            throw std::runtime_error{"No registers available"};
        }

        return InstructionResultInfo{std::move(reg), flags};
    }

    InstructionResultInfo Target6502Asm::EmitInt8Copy(
            Register const &source, BinaryOutputStream &out
    ) {
        Register           reg{RegisterAllocate(registers::Size::Byte)};
        CompareFlags       flags{};
        std::uint8_t const srcZeroPageReg{
                m_RegisterInfo.at(source.GetId().value)->GetIndex(0)
        };
        std::uint8_t const destZeroPageReg{
                m_RegisterInfo.at(reg.GetId().value)->GetIndex(0)
        };

        if (!m_RegistersOccupied.a) {
            out << printer::load::LdaZeroPage(srcZeroPageReg, flags)
                << printer::store::StaZeroPage(destZeroPageReg, flags);
        } else if (!m_RegistersOccupied.x) {
            out << printer::load::LdxZeroPage(srcZeroPageReg, flags)
                << printer::store::StxZeroPage(destZeroPageReg, flags);
        } else if (!m_RegistersOccupied.y) {
            out << printer::load::LdyZeroPage(srcZeroPageReg, flags)
                << printer::store::StyZeroPage(destZeroPageReg, flags);
        } else {
            throw std::runtime_error{"No registers available"};
        }

        return InstructionResultInfo{std::move(reg), flags};
    }

    InstructionResultInfo Target6502Asm::EmitLogicalAndInt8Immediate(
            Register const &lhs, std::int8_t rhs, BinaryOutputStream &out
    ) {
        Register           reg{RegisterAllocate(registers::Size::Byte)};
        CompareFlags       flags{};
        std::uint8_t const resultZeroPageReg{
                m_RegisterInfo.at(reg.GetId().value)->GetIndex(0)
        };
        std::uint8_t const lhsZeroPageReg{
                m_RegisterInfo.at(lhs.GetId().value)->GetIndex(0)
        };

        out << printer::load::LdaZeroPage(lhsZeroPageReg, flags)
            << printer::bitwise::AndImmediate(rhs, flags)
            << printer::store::StaZeroPage(resultZeroPageReg, flags);

        return InstructionResultInfo{std::move(reg), flags};
    }

    InstructionResultInfo Target6502Asm::EmitLogicalAndInt8Register(
            Register const &lhs, Register const &rhs, BinaryOutputStream &out
    ) {
        Register           reg{RegisterAllocate(registers::Size::Byte)};
        CompareFlags       flags{};
        std::uint8_t const resultZeroPageReg{
                m_RegisterInfo.at(reg.GetId().value)->GetIndex(0)
        };
        std::uint8_t const lhsZeroPageReg{
                m_RegisterInfo.at(lhs.GetId().value)->GetIndex(0)
        };
        std::uint8_t const rhsZeroPageReg{
                m_RegisterInfo.at(rhs.GetId().value)->GetIndex(0)
        };

        out << printer::load::LdaZeroPage(lhsZeroPageReg, flags)
            << printer::bitwise::AndZeroPage(rhsZeroPageReg, flags)
            << printer::store::StaZeroPage(resultZeroPageReg, flags);

        return InstructionResultInfo{std::move(reg), flags};
    }

    InstructionResultInfo
    Target6502Asm::EmitIncrement(Register const &reg, BinaryOutputStream &out) {
        CompareFlags       flags{};
        std::uint8_t const resultZeroPageReg{
                m_RegisterInfo.at(reg.GetId().value)->GetIndex(0)
        };

        out << printer::arithmetic::IncrementZeroPage(resultZeroPageReg, flags);

        return InstructionResultInfo{std::nullopt, flags};
    }

    void Target6502Asm::FreeZeroPageRegister(ZpRegister value) {
        m_ZeroPageOccupied[value] = false;
    }

    Register Target6502Asm::RegisterAllocate(registers::Size size) {
        std::cout << "Allocating register of size " << static_cast<int>(size)
                  << '\n';
        ZpRegister6502::Indices regIndices{};

        auto const sizeRequested{static_cast<int>(size)};
        auto       bytesAllocated{0};

        for (std::size_t i = 0; i < m_ZeroPageOccupied.size(); ++i) {
            if (!m_ZeroPageOccupied[i]) {
                m_ZeroPageOccupied[i]      = true;
                regIndices[bytesAllocated] = i;
                ++bytesAllocated;
            }

            if (bytesAllocated != sizeRequested)
                continue;

            auto const regIdx{[&] {
                auto const firstFree{std::ranges::find_if(
                        m_RegisterInfo,
                        [](auto const &regInfo) { return !regInfo.has_value(); }
                )};

                if (firstFree == m_RegisterInfo.end()) {
                    m_RegisterInfo.emplace_back(
                            ZpRegister6502{*this, size, regIndices}
                    );
                    return m_RegisterInfo.size() - 1;
                }

                firstFree->emplace(*this, size, regIndices);
                return static_cast<std::size_t>(
                        std::distance(m_RegisterInfo.begin(), firstFree)
                );
            }()};

            return Register{*this, {regIdx}, size};
        }

        // TODO: proper exception
        throw std::runtime_error{"No zero page registers available"};
    }

    void Target6502Asm::RegisterFree(registers::Id reg) {
        m_RegisterInfo.at(reg.value).reset();
    }

}// namespace jcc::codegen::asm6502
