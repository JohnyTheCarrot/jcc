#ifndef ERROR_COMMAND_H
#define ERROR_COMMAND_H

#include "command.h"

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
