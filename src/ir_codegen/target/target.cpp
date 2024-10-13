#include "target.h"

namespace jcc::codegen {
    CompareFlags CompareFlags::operator|(CompareFlags const &rhs) const {
        return CompareFlags{
                .zero     = zero || rhs.zero,
                .sign     = sign || rhs.sign,
                .carry    = carry || rhs.carry,
                .overflow = overflow || rhs.overflow,
                .negative = negative || rhs.negative
        };
    }

    CompareFlags &CompareFlags::operator|=(CompareFlags const &rhs) {
        *this = *this | rhs;
        return *this;
    }

    Register const *InstructionResultInfo::operator->() const {
        return &reg.value();
    }

    Register *InstructionResultInfo::operator->() {
        return &reg.value();
    }
}// namespace jcc::codegen
