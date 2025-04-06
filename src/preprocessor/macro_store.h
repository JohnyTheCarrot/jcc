#ifndef MACRO_STORE_H
#define MACRO_STORE_H

#include <optional>     // for optional
#include <stack>        // for stack
#include <string>       // for hash, string, operator==
#include <unordered_map>// for unordered_map
#include <vector>       // for vector

#include "macro.h"          // for MacroInvocation, Macro
#include "tokenizer/token.h"// for Token

namespace jcc::preprocessor {
    class MacroStore final {
        using MacroDefinitions = std::unordered_map<std::string, macro::Macro>;

        MacroDefinitions                   m_MacroDefinitions{};
        std::stack<macro::MacroInvocation> m_MacroStack{};

    public:
        MacroStore();

        [[nodiscard]]
        std::size_t GetMacroStackSize() const;

        [[nodiscard]]
        bool IsMacroDefined(std::string const &name) const;

        void UnregisterMacro(std::string const &ident);

        void RegisterMacro(std::string const &name, macro::Macro macro);

        void InvokeMacro(macro::MacroInvocation &&macroInvocation);

        [[nodiscard]]
        macro::MacroInvocation *GetCurrentMacroInvocation() noexcept;

        [[nodiscard]]
        macro::Macro const *
        GetMacro(std::string const &name, bool allowCurrentMacro = false) const;

        [[nodiscard]]
        std::optional<std::vector<tokenizer::Token>>
        GetMacroArgument(std::string const &argumentName) const;

        [[nodiscard]]
        std::optional<PreprocessorToken> GetTokenFromMacroStack();

        [[nodiscard]]
        std::optional<tokenizer::Token> GetTokenFromMacroArgumentReader();

        void PushMacroArgumentTokens(std::vector<tokenizer::Token> &&args);
    };
}// namespace jcc::preprocessor

#endif//MACRO_STORE_H
