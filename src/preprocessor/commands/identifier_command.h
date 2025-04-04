#ifndef IDENTIFIER_COMMAND_H
#define IDENTIFIER_COMMAND_H

#include <optional>// for optional

#include "command.h"                        // for Command, CommandMap
#include "preprocessor/preprocessor_token.h"// for PreprocessorToken

namespace jcc {
    namespace tokenizer {
        struct Token;
    }// namespace tokenizer
}// namespace jcc

namespace jcc::preprocessor {
    class Preprocessor;
}

namespace jcc::preprocessor::commands {
    class IdentifierCommand final : public Command {
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
