#include "type.h"

#include <cmath>
#include <magic_enum/magic_enum.hpp>

#include "parsing_sema/parser.h"

namespace jcc::parsing_sema::types {
    BitInteger::BitInteger(int bitWidth)
        : m_BitWidth{bitWidth} {
    }

    int BitInteger::GetBitWidth() const noexcept {
        return m_BitWidth;
    }

    llvm::Type *BitInteger::GetLLVMType() const {
        auto &context{CompilerState::GetInstance().GetContext()};

        return llvm::Type::getIntNTy(context, m_BitWidth);
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

    bool IntegerType::IsSigned() const noexcept {
        return m_Sign == Signedness::Signed;
    }

    llvm::Type *IntegerType::GetLLVMType() const {
        auto &context{CompilerState::GetInstance().GetContext()};

        if (std::holds_alternative<StandardIntegerType>(m_Type)) {
            switch (std::get<StandardIntegerType>(m_Type)) {
                case StandardIntegerType::Char:
                    return llvm::Type::getInt8Ty(context);
                case StandardIntegerType::Short:
                    return llvm::Type::getInt16Ty(context);
                case StandardIntegerType::Int:
                    return llvm::Type::getInt32Ty(context);
                case StandardIntegerType::Long:
                case StandardIntegerType::LongLong:
                    return llvm::Type::getInt64Ty(context);
                default:
                    assert(false);
                    return nullptr;
            }
        }

        if (std::holds_alternative<BitInteger>(m_Type)) {
            return std::get<BitInteger>(m_Type).GetLLVMType();
        }

        assert(false);
        return nullptr;
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
