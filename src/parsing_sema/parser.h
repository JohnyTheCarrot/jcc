#ifndef PARSER_STATE_H
#define PARSER_STATE_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>

#include "ast_node.h"
#include "tokenizer/token.h"

namespace jcc::parsing_sema {
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
    };
}// namespace jcc::parsing_sema

#endif//PARSER_STATE_H
