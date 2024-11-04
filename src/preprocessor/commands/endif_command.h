#ifndef ENDIF_COMMAND_H
#define ENDIF_COMMAND_H

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
