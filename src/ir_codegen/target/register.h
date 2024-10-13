#ifndef REGISTER_H
#define REGISTER_H

#include <cstddef>
#include <limits>
#include <memory>

#include "common.h"

namespace jcc::codegen {
    struct InstructionResultInfo;

    class Target;

    namespace registers {
        struct Id {
            std::size_t value;

            [[nodiscard]]
            bool
            operator==(Id const &rhs) const;

            [[nodiscard]]
            bool
            operator==(std::nullptr_t) const;

            Id &operator=(Id const &);

            Id &operator=(std::nullptr_t);

            [[nodiscard]] operator bool() const;
        };

        constexpr auto NullId = std::numeric_limits<std::size_t>::max();

        enum class Size {
            Byte  = 1,
            Word  = 2,
            DWord = 4,
            QWord = 8,
        };
        constexpr auto c_MaxSize = static_cast<std::size_t>(Size::QWord);
    }// namespace registers

    class RegisterDestroyer final {
        Target *m_Target;

    public:
        using pointer = registers::Id;

        explicit RegisterDestroyer(Target &target);

        void operator()(registers::Id id) const;

        [[nodiscard]]
        Target &GetTarget() const;
    };

    class Register final {
        std::unique_ptr<void, RegisterDestroyer> m_Id;
        registers::Size                          m_Size;

        [[nodiscard]]
        Target &GetTarget() const;

    public:
        Register(Target &target, registers::Id id, registers::Size size);

        [[nodiscard]]
        registers::Id GetId() const;

        [[nodiscard]]
        InstructionResultInfo Copy(BinaryOutputStream &out) const;

        [[nodiscard]]
        InstructionResultInfo
        LogicalAnd(Register const &rhs, BinaryOutputStream &out) const;

        [[nodiscard]]
        InstructionResultInfo
        LogicalAnd(std::int8_t rhs, BinaryOutputStream &out) const;

        [[nodiscard]]
        InstructionResultInfo Increment(BinaryOutputStream &out) const;
    };
}// namespace jcc::codegen

#endif//REGISTER_H
