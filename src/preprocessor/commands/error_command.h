#ifndef ERROR_COMMAND_H
#define ERROR_COMMAND_H

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
    class ErrorCommand final : public Command {
    public:
        explicit ErrorCommand(CommandMap &map);

        ~ErrorCommand() override;

        [[nodiscard]]
        std::optional<PreprocessorToken>
        Execute(Preprocessor      &preprocessor,
                tokenizer::Token &&directive) const override;
    };
}// namespace jcc::preprocessor::commands

#endif//ERROR_COMMAND_H
