#include "parser.h"

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
}// namespace jcc::parsing_sema
