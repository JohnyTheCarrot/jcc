#ifndef ELSE_COMMAND_H
#define ELSE_COMMAND_H

#include "command.h"

namespace jcc::preprocessor {
    class Preprocessor;
}

namespace jcc::preprocessor::commands {
    class ElseCommand final : public Command {
    public:
        explicit ElseCommand(CommandMap &map);

        ~ElseCommand() override;

        [[nodiscard]]
        std::optional<PreprocessorToken>
        Execute(Preprocessor &preprocessor, tokenizer::Token &&) const override;
    };
}// namespace jcc::preprocessor::commands

#endif//ELSE_COMMAND_H
