#include "parser.h"

#include <iostream>

#include "diagnostics/variants/visitors/mjolnir_visitor.hpp"

namespace jcc::parsing {
    CompilerState &CompilerState::GetInstance() {
        static CompilerState instance{};
        return instance;
    }

    bool CompilerState::HasFatalError() const noexcept {
        return m_HasFatalError;
    }

    void CompilerState::PrintDiagnostics(DiagnosticsFormat) const {
        diagnostics::MjolnirVisitor visitor{std::cout};

        for (auto const &diagnosis : m_Diagnostics) {
            diagnosis->Visit(visitor);
        }
    }
}// namespace jcc::parsing
