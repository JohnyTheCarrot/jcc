#ifndef PARSER_STATE_H
#define PARSER_STATE_H

#include <concepts>                   // for constructible_from, derived_from
#include <iterator>                   // for iter_value_t, input_iterator
#include <llvm/IR/IRBuilder.h>        // for IRBuilder
#include <llvm/IR/LLVMContext.h>      // for LLVMContext
#include <llvm/MC/TargetRegistry.h>   // for Target (ptr only), TargetRegi...
#include <llvm/Support/TargetSelect.h>// for InitializeAllTargetInfos, Ini...
#include <llvm/TargetParser/Host.h>   // for getDefaultTargetTriple
#include <memory>                     // for make_unique
#include <stdexcept>                  // for runtime_error
#include <string>                     // for string
#include <utility>                    // for forward
#include <vector>                     // for vector

#include "diagnostics/diagnostics.h"// for FatalCompilerError, Diagnosti...

namespace jcc {
    namespace tokenizer {
        struct Token;
    }// namespace tokenizer
}// namespace jcc

namespace jcc::parsing {
    // TODO: this isn't really only for parsing and semantic analysis
    template<typename TIterator>
    concept TokenIterator =
            std::input_iterator<TIterator> and
            std::same_as<std::iter_value_t<TIterator>, tokenizer::Token>;

    class CompilerState final {
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

        std::vector<diagnostics::Diagnostic> m_Diagnostics{};
        bool                                 m_HasFatalError{false};

    public:
        [[nodiscard]]
        llvm::IRBuilder<> &GetBuilder() noexcept;

        [[nodiscard]]
        llvm::IRBuilder<> const &GetBuilder() const noexcept;

        [[nodiscard]]
        llvm::LLVMContext &GetContext() noexcept;

        [[nodiscard]]
        llvm::LLVMContext const &GetContext() const noexcept;

        static CompilerState &GetInstance();

        /**
         * 
         * @tparam D    The diagnostic type
         * @tparam Args The arguments to the diagnostic constructor
         * @param  args The arguments to the diagnostic constructor
         */
        template<class D, class... Args>
            requires std::derived_from<D, diagnostics::DiagnosticData> and
                     std::constructible_from<D, Args...>
        void EmplaceDiagnostic(Args &&...args) {
            auto diagnostic{std::make_unique<D>(std::forward<Args>(args)...)};
            if (diagnostic->IsError()) {
                m_HasFatalError = true;
            }

            m_Diagnostics.emplace_back(std::move(diagnostic));
        }

        /**
         * Emplace a diagnostic and throw a FatalCompilerError.
         *
         * Usage of this function should be reserved for when the compiler encounters an error that it cannot recover from.
         * 
         * @tparam D 
         * @tparam Args 
         * @param args
         * @throws FatalCompilerError
         */
        template<class D, class... Args>
            requires std::derived_from<D, diagnostics::DiagnosticData> and
                     std::constructible_from<D, Args...>
        [[noreturn]]
        void EmplaceFatalDiagnostic(Args &&...args) {
            EmplaceDiagnostic<D>(std::forward<Args>(args)...);
            throw diagnostics::FatalCompilerError{};
        }

        /**
         * Emplace a diagnostic and throw a FatalCompilerError.
         *
         * Usage of this function implies the programmer believes it's possible to recover from the error, but the infrastructure is not in place to do so yet.
         * Fatal means that the compiler cannot recover from the error and must stop execution.
         * 
         * @tparam D                    The diagnostic type
         * @tparam Args                 The arguments to the diagnostic constructor
         * @param  args                 The arguments to the diagnostic constructor
         * @throws FatalCompilerError   Unconditionally
         */
        template<class D, class... Args>
            requires std::derived_from<D, diagnostics::DiagnosticData> and
                     std::constructible_from<D, Args...>
        [[noreturn]]
        void EmplaceTemporarilyFatalDiagnostic(Args &&...args) {
            EmplaceDiagnostic<D>(std::forward<Args>(args)...);
            throw diagnostics::FatalCompilerError{};
        }

        [[nodiscard]]
        bool HasFatalError() const noexcept;

        enum class DiagnosticsFormat {
            Fancy,
            // Compact,
            // SARIF,
        };

        /**
         * Print the diagnostics to the given stream.
         * 
         * @param format  The format to print the diagnostics in
         */
        void PrintDiagnostics(DiagnosticsFormat format) const;
    };
}// namespace jcc::parsing

#endif//PARSER_STATE_H
