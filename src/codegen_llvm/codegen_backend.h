#ifndef CODEGEN_LLVM_BACKEND_H
#define CODEGEN_LLVM_BACKEND_H

#include <memory>
#include <string>

namespace jcc::parsing {
    class AstExpression;
}

namespace jcc::codegen_llvm {
    class LLVMCodegenContext;

    class LLVMCodegenBackend {
        std::unique_ptr<LLVMCodegenContext> m_Context;

    public:
        LLVMCodegenBackend();
        ~LLVMCodegenBackend();

        LLVMCodegenBackend(LLVMCodegenBackend const &) = delete;
        LLVMCodegenBackend &operator=(LLVMCodegenBackend const &) = delete;
        LLVMCodegenBackend(LLVMCodegenBackend &&) noexcept;
        LLVMCodegenBackend &operator=(LLVMCodegenBackend &&) noexcept;

        void EmitExpression(std::string const &moduleName, parsing::AstExpression const *expression);
    };
} // namespace jcc::codegen_llvm

#endif // CODEGEN_LLVM_BACKEND_H
