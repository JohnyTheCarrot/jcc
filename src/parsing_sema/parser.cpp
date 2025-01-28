#include "parser.h"

#include <iostream>

namespace jcc::parsing_sema {
    CompilerState::~CompilerState() {
        for (auto const &diagnosis : m_WarningDiagnoses) {
            std::cout << diagnosis.GetMessage() << '\n';
        }

        for (auto const &diagnosis : m_ErrorDiagnoses) {
            std::cout << diagnosis.GetMessage() << '\n';
        }
    }

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
        return !m_ErrorDiagnoses.empty();
    }
}// namespace jcc::parsing_sema
