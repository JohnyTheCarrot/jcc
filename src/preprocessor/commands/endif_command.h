#ifndef ENDIF_COMMAND_H
#define ENDIF_COMMAND_H

#include "command.h"

namespace jcc::preprocessor {
    class Preprocessor;
}

namespace jcc::preprocessor::commands {
    class EndifCommand final : public Command {
    public:
        explicit EndifCommand(CommandMap &map);

        ~EndifCommand() override;

        [[nodiscard]]
        std::optional<PreprocessorToken>
        Execute(Preprocessor &preprocessor, tokenizer::Token &&) const override;
    };
}// namespace jcc::preprocessor::commands

#endif//ENDIF_COMMAND_H
