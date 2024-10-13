#include "register.h"

#include "target.h"

namespace jcc::codegen {
    namespace registers {
        bool Id::operator==(Id const &rhs) const {
            return value == rhs.value;
        }

        bool Id::operator==(std::nullptr_t) const {
            return value == NullId;
        }

        Id &Id::operator=(Id const &id) = default;

        Id &Id::operator=(std::nullptr_t) {
            value = NullId;
            return *this;
        }

        Id::operator bool() const {
            return value != NullId;
        }
    }// namespace registers

    RegisterDestroyer::RegisterDestroyer(Target &target)
        : m_Target{&target} {
    }

    void RegisterDestroyer::operator()(registers::Id id) const {
        if (id == nullptr)
            return;

        m_Target->RegisterFree(id);
    }

    Target &RegisterDestroyer::GetTarget() const {
        return *m_Target;
    }

    Target &Register::GetTarget() const {
        return m_Id.get_deleter().GetTarget();
    }

    Register::Register(Target &target, registers::Id id, registers::Size size)
        : m_Id{id, RegisterDestroyer{target}}
        , m_Size{size} {
    }

    registers::Id Register::GetId() const {
        return m_Id.get();
    }

    InstructionResultInfo Register::Copy(BinaryOutputStream &out) const {
        auto &target{GetTarget()};

        switch (m_Size) {
            case registers::Size::Byte:
                return target.EmitInt8Copy(*this, out);
            default:
                // TODO
                throw std::runtime_error{"Unsupported register size"};
        }
    }

    InstructionResultInfo
    Register::LogicalAnd(Register const &rhs, BinaryOutputStream &out) const {
        auto &target{GetTarget()};

        switch (m_Size) {
            case registers::Size::Byte:
                return target.EmitLogicalAndInt8Register(*this, rhs, out);
            default:
                // TODO
                throw std::runtime_error{"Unsupported register size"};
        }
    }

    InstructionResultInfo
    Register::LogicalAnd(std::int8_t rhs, BinaryOutputStream &out) const {
        auto &target{GetTarget()};

        switch (m_Size) {
            case registers::Size::Byte:
                return target.EmitLogicalAndInt8Immediate(*this, rhs, out);
            default:
                // TODO
                throw std::runtime_error{"Unsupported register size"};
        }
    }

    InstructionResultInfo Register::Increment(BinaryOutputStream &out) const {
        auto &target{GetTarget()};

        switch (m_Size) {
            case registers::Size::Byte:
                return target.EmitIncrement(*this, out);
            default:
                // TODO
                throw std::runtime_error{"Unsupported register size"};
        }
    }
}// namespace jcc::codegen
