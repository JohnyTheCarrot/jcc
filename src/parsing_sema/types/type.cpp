#include "type.h"

#include <cmath>
#include <magic_enum/magic_enum.hpp>

namespace jcc::parsing_sema::types {
    BitInteger::BitInteger(int bitWidth)
        : m_BitWidth{bitWidth} {
    }

    int BitInteger::GetBitWidth() const noexcept {
        return m_BitWidth;
    }

    void PrintTo(BitInteger const &bitInteger, std::ostream *os) {
        *os << "BitInteger{" << bitInteger.GetBitWidth() << '}';
    }

    void PrintTo(StandardIntegerType const &bitInteger, std::ostream *os) {
        *os << "StandardIntegerType::" << magic_enum::enum_name(bitInteger);
    }

    bool IntegerType::operator==(IntegerType const &other) const {
        return m_Type == other.m_Type && m_Sign == other.m_Sign;
    }

    void PrintTo(IntegerType type, std::ostream *os) {
        *os << "IntegerType{";
        std::visit([&os](auto const &t) { PrintTo(t, os); }, type.m_Type);
        *os << ", " << magic_enum::enum_name(type.m_Sign) << '}';
    }

    IntegerLimits::IntegerLimits(NumBits numBits)
        : m_NumBits{numBits} {
    }

    IntegerLimits::NumBytes IntegerLimits::GetNumBytes() const noexcept {
        return static_cast<NumBytes>(
                std::ceil(static_cast<float>(m_NumBits) / 8.f)
        );
    }

    IntegerLimits::NumBits IntegerLimits::GetNumBits() const noexcept {
        return m_NumBits;
    }
}// namespace jcc::parsing_sema::types
