#include "parser.h"

#include <iostream>

#include "diagnostics/variants/visitors/mjolnir_visitor.hpp"

namespace jcc::parsing_sema {
    llvm::IRBuilder<> &CompilerState::GetBuilder() noexcept {
        return m_Builder;
    }

    llvm::IRBuilder<> const &CompilerState::GetBuilder() const noexcept {
        return m_Builder;
    }

    llvm::LLVMContext &CompilerState::GetContext() noexcept {
        return m_Context;
    }

    llvm::LLVMContext const &CompilerState::GetContext() const noexcept {
        return m_Context;
    }

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
}// namespace jcc::parsing_sema
