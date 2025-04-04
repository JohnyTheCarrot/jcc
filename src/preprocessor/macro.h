#ifndef MACRO_H
#define MACRO_H

#include <iosfwd>       // for ostream
#include <optional>     // for optional
#include <string>       // for string
#include <unordered_map>// for unordered_map
#include <variant>      // for variant
#include <vector>       // for vector

#include "misc/Span.h"      // for Span
#include "tokenizer/token.h"// for Token, Identifier

namespace jcc::preprocessor::macro {
    struct ReplacementList {
        using TokenList = std::vector<tokenizer::Token>;
        TokenList m_ReplacementList{};

        [[nodiscard]]
        bool operator==(ReplacementList const &other) const noexcept;
    };

    struct ObjectLikeMacro final {
        std::string     m_MacroName;
        Span            m_Span;
        ReplacementList m_ReplacementList;

        ObjectLikeMacro(
                std::string macroName, Span span,
                ReplacementList replacementList
        );

        [[nodiscard]]
        bool operator==(ObjectLikeMacro const &other) const noexcept;
    };

    struct FunctionLikeMacro final {
        using ParameterList = std::vector<tokenizer::Identifier>;

        std::string     m_MacroName;
        Span            m_Span;
        ReplacementList m_ReplacementList;
        ParameterList   m_ParameterList;
        bool            m_IsVA;

        FunctionLikeMacro(
                std::string macroName, Span span,
                ReplacementList &&replacementList,
                ParameterList &&parameterList, bool isVA
        );

        [[nodiscard]]
        bool operator==(FunctionLikeMacro const &other) const noexcept;
    };

    using Macro = std::variant<ObjectLikeMacro, FunctionLikeMacro>;

    using FnMacroArguments =
            std::unordered_map<std::string, std::vector<tokenizer::Token>>;

    std::ostream &operator<<(std::ostream &os, FnMacroArguments const &args);

    struct MacroArgumentReader final {
        std::vector<tokenizer::Token> m_Args{};
        int                           m_CurrentTokenIndex{-1};
    };

    struct MacroInvocation final {
        std::string                        m_MacroName{};
        int                                m_CurrentToken{-1};
        std::optional<FnMacroArguments>    m_Args{};
        std::optional<MacroArgumentReader> m_CurrentArgReader{};
    };
}// namespace jcc::preprocessor::macro

#endif//MACRO_H
