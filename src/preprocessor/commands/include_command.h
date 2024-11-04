#ifndef INCLUDE_COMMAND_H
#define INCLUDE_COMMAND_H

#include <optional>// for optional

#include "command.h"                        // for Command, CommandMap
#include "preprocessor/preprocessor_token.h"// for PreprocessorToken

namespace jcc::tokenizer {
    struct Token;
}// namespace jcc::tokenizer

namespace jcc::preprocessor {
    class Preprocessor;
}

namespace jcc::preprocessor::commands {
    class IncludeCommand final : public Command {
    public:
        explicit IncludeCommand(CommandMap &map);

        ~IncludeCommand() override;

        [[nodiscard]]
        std::optional<PreprocessorToken>
        Execute(Preprocessor      &preprocessor,
                tokenizer::Token &&directive) const override;
    };
}// namespace jcc::preprocessor::commands

#endif//INCLUDE_COMMAND_H
