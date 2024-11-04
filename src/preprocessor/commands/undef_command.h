#ifndef UNDEF_COMMAND_H
#define UNDEF_COMMAND_H

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
    class UndefCommand final : public Command {
    public:
        explicit UndefCommand(CommandMap &map);

        ~UndefCommand() override;

        [[nodiscard]]
        std::optional<PreprocessorToken>
        Execute(Preprocessor      &preprocessor,
                tokenizer::Token &&ident) const override;
    };
}// namespace jcc::preprocessor::commands

#endif//UNDEF_COMMAND_H
