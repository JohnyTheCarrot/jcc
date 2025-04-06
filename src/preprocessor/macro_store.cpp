#include "macro_store.h"

#include <cassert>
#include <iostream>
#include <ranges>   // for __find_if_fn, find_if
#include <stdexcept>// for runtime_error
#include <utility>  // for move, pair
#include <variant>  // for get, holds_alternative
#include <vector>   // for vector

#include "preprocessor/macro.h"// for MacroInvocation, MacroArgumentReader

namespace jcc::preprocessor {
    MacroStore::MacroStore() = default;

    std::size_t MacroStore::GetMacroStackSize() const {
        return m_MacroStack.size();
    }

    bool MacroStore::IsMacroDefined(std::string const &name) const {
        return m_MacroDefinitions.contains(name);
    }

    void MacroStore::UnregisterMacro(std::string const &ident) {
        m_MacroDefinitions.erase(ident);
    }

    void
    MacroStore::RegisterMacro(std::string const &name, macro::Macro macro) {
        m_MacroDefinitions.emplace(name, std::move(macro));
    }

    void MacroStore::InvokeMacro(macro::MacroInvocation &&macroInvocation) {
        using macro::operator<<;
        std::cout << "Invoking macro: " << macroInvocation.m_MacroName << '\n';

        if (macroInvocation.m_Args.has_value())
            std::cout << "Arguments:\n"
                      << macroInvocation.m_Args.value() << '\n';

        m_MacroStack.push(macroInvocation);
    }

    macro::MacroInvocation *MacroStore::GetCurrentMacroInvocation() noexcept {
        if (m_MacroStack.empty())
            return nullptr;

        return &m_MacroStack.top();
    }

    macro::Macro const *MacroStore::GetMacro(
            std::string const &name, bool allowCurrentMacro
    ) const {
        if (auto const macroIt{std::ranges::find_if(
                    m_MacroDefinitions,
                    [&](auto &pair) {
                        if (!m_MacroStack.empty()) {
                            auto const &topInvokedMacro{m_MacroStack.top()};
                            if (pair.first == topInvokedMacro.m_MacroName)
                                return allowCurrentMacro;
                        }

                        return pair.first == name;
                    }
            )};
            macroIt != m_MacroDefinitions.end()) {
            return &macroIt->second;
        }

        return nullptr;
    }

    std::optional<std::vector<tokenizer::Token>>
    MacroStore::GetMacroArgument(std::string const &argumentName) const {
        if (m_MacroStack.empty())
            return std::nullopt;

        auto const macroTop{m_MacroStack.top()};
        if (!macroTop.m_Args.has_value())
            return std::nullopt;

        auto const argIt{std::ranges::find_if(
                macroTop.m_Args.value(),
                [&](auto const &arg) { return arg.first == argumentName; }
        )};

        if (argIt == macroTop.m_Args.value().end())
            return std::nullopt;

        return argIt->second;
    }

    std::optional<PreprocessorToken> MacroStore::GetTokenFromMacroStack() {
        if (m_MacroStack.empty())
            return std::nullopt;

        auto &[macroName, currentTokenIndex, args, currentArgReader,
               nextToken]{m_MacroStack.top()};
        ++currentTokenIndex;

        if (auto const *macro{GetMacro(macroName, true)}; macro != nullptr) {
            auto const [m_ReplacementList]{[macro] {
                if (std::holds_alternative<macro::ObjectLikeMacro>(*macro)) {
                    return std::get<macro::ObjectLikeMacro>(*macro)
                            .m_ReplacementList;
                }

                return std::get<macro::FunctionLikeMacro>(*macro)
                        .m_ReplacementList;
            }()};

            if (currentTokenIndex >=
                static_cast<int>(m_ReplacementList.size())) {
                std::cout << "Returning from macro: " << macroName << std::endl;
                if (currentArgReader.has_value()) {
                    std::cout << "Macro had " << currentArgReader->m_Args.size()
                              << " args and we are at "
                              << currentArgReader->m_CurrentTokenIndex
                              << std::endl;
                    assert(static_cast<int>(currentArgReader->m_Args.size()) <=
                           currentArgReader->m_CurrentTokenIndex);
                }
                m_MacroStack.pop();
                if (auto argToken{GetTokenFromMacroArgumentReader()};
                    argToken.has_value())
                    return BasicPreprocessorToken{std::move(argToken.value())};

                return GetTokenFromMacroStack();
            }

            return m_ReplacementList.at(currentTokenIndex);
        }

        throw std::runtime_error{
                "Macro stack is not empty, but the macro is not found"
        };
    }

    std::optional<tokenizer::Token>
    MacroStore::GetTokenFromMacroArgumentReader() {
        if (m_MacroStack.empty())
            return std::nullopt;

        auto &macroArgReader{m_MacroStack.top().m_CurrentArgReader};

        if (!macroArgReader.has_value())
            return std::nullopt;

        auto &[args, currentTokenIndex]{macroArgReader.value()};

        ++currentTokenIndex;

        if (currentTokenIndex >= static_cast<int>(args.size())) {
            macroArgReader = std::nullopt;
            return std::nullopt;
        }

        return args.at(currentTokenIndex);
    }

    void
    MacroStore::PushMacroArgumentTokens(std::vector<tokenizer::Token> &&args) {
        if (m_MacroStack.empty())
            throw std::runtime_error{
                    "Macro argument reader is set, but the macro stack is empty"
            };

        auto &currentMacro{m_MacroStack.top()};
        currentMacro.m_CurrentArgReader =
                macro::MacroArgumentReader{std::move(args)};
    }
}// namespace jcc::preprocessor
