#ifndef IDENTIFIER_COMMAND_H
#define IDENTIFIER_COMMAND_H

#include <optional>// for optional
#include <utility> // for pair
#include <vector>  // for vector

#include "command.h"                        // for Command, CommandMap
#include "preprocessor/macro.h"             // for FnMacroArguments, Funct...
#include "preprocessor/preprocessor_token.h"// for PreprocessorToken
#include "tokenizer/token.h"                // for Token

namespace jcc::preprocessor {
    class Preprocessor;
}

namespace jcc::preprocessor::commands {
    class IdentifierCommand final : public Command {
        [[nodiscard]]
        static macro::FnMacroArguments GatherArguments(
                Preprocessor                   &preprocessor,
                macro::FunctionLikeMacro const &fnMacro
        );

        [[nodiscard]]
        static std::pair<bool, std::vector<tokenizer::Token>>
        GatherArgumentTokens(Preprocessor &preprocessor);

    public:
        explicit IdentifierCommand(CommandMap &map);

        ~IdentifierCommand() override;

        [[nodiscard]]
        std::optional<PreprocessorToken>
        Execute(Preprocessor      &preprocessor,
                tokenizer::Token &&ident) const override;
    };
}// namespace jcc::preprocessor::commands

#endif//IDENTIFIER_COMMAND_H
