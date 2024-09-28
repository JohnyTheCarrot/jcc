#ifndef DEFINE_COMMAND_H
#define DEFINE_COMMAND_H

#include <utility>
#include <string>

#include "command.h"
#include "preprocessor/macro.h"

namespace jcc::preprocessor::commands {
    class DefineCommand final : public Command {
        [[nodiscard]]
        static macro::ReplacementList
        GatherReplacementList(Preprocessor &preprocessor);

        [[nodiscard]]
        /// \return a pair of whether the parameter list is variadic and the parameter list
        static std::pair<bool, macro::FunctionLikeMacro::ParameterList>
        GatherParameterList(Preprocessor &preprocessor);

        static void DefineObjectLikeMacro(
                Preprocessor &preprocessor, std::string &&name,
                tokenizer::Token &&firstToken
        );

        static void
        DefineFunctionLikeMacro(Preprocessor &preprocessor, std::string &&name);

    public:
        explicit DefineCommand(CommandMap &map);

        ~DefineCommand() override;

        [[nodiscard]]
        std::optional<PreprocessorToken>
        Execute(Preprocessor &preprocessor, tokenizer::Token &&) const override;
    };
}// namespace jcc::preprocessor::commands

#endif//DEFINE_COMMAND_H
