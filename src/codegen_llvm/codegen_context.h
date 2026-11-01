#ifndef CODEGEN_LLVM_CONTEXT_H
#define CODEGEN_LLVM_CONTEXT_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <stdexcept>
#include <string>

namespace jcc::codegen_llvm {
    class LLVMCodegenContext {
        llvm::LLVMContext   m_Context{};
        llvm::IRBuilder<>   m_Builder{m_Context};
        llvm::Target const *m_Target{[] {
            llvm::InitializeAllTargetInfos();
            llvm::InitializeAllTargets();
            auto const targetTriple{llvm::sys::getDefaultTargetTriple()};

            std::string error;
            if (auto const target{
                        llvm::TargetRegistry::lookupTarget(targetTriple, error)
                };
                target != nullptr) {
                return target;
            }

            throw std::runtime_error{error};
        }()};

    public:
        [[nodiscard]]
        llvm::IRBuilder<> &GetBuilder() noexcept {
            return m_Builder;
        }

        [[nodiscard]]
        llvm::IRBuilder<> const &GetBuilder() const noexcept {
            return m_Builder;
        }

        [[nodiscard]]
        llvm::LLVMContext &GetContext() noexcept {
            return m_Context;
        }

        [[nodiscard]]
        llvm::LLVMContext const &GetContext() const noexcept {
            return m_Context;
        }

        [[nodiscard]]
        llvm::Target const *GetTarget() const noexcept {
            return m_Target;
        }
    };
} // namespace jcc::codegen_llvm

#endif // CODEGEN_LLVM_CONTEXT_H
