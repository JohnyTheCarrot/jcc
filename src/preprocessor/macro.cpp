#include "macro.h"

#include <ostream>// for basic_ostream, operator<<, basic_ios, ostream
#include <utility>// for pair

#include "gtest/gtest.h"// for PrintToString

namespace jcc::preprocessor::macro {
    MacroDefinition::MacroDefinition(
            std::string macroName, Span span, ReplacementList replacementList
    ) noexcept
        : m_MacroName{std::move(macroName)}
        , m_Span{std::move(span)}
        , m_ReplacementList{std::move(replacementList)} {
    }

    FunctionLikeMacro::FunctionLikeMacro(
            std::string macroName, Span span, ReplacementList &&replacementList,
            ParameterList &&parameterList, bool isVA
    )
        : MacroDefinition{std::move(macroName), std::move(span), std::move(replacementList)}
        , m_ParameterList{std::move(parameterList)}
        , m_IsVA{isVA} {
    }

    std::ostream &operator<<(std::ostream &os, FnMacroArguments const &args) {
        for (auto const &[name, value] : args) {
            os << testing::PrintToString(name) << " -> ";
            os << testing::PrintToString(value) << '\n';
        }

        return os;
    }
}// namespace jcc::preprocessor::macro
