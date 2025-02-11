#ifndef IFDEF_COMMAND_H
#define IFDEF_COMMAND_H
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
    class IfdefCommand final : public Command {
        bool m_IsIfndef;

        static void ExecuteCondition(
                bool isIfndef, Preprocessor &preprocessor, Span const &span
        );

    public:
        explicit IfdefCommand(CommandMap &map, bool isIfndef = false);

        ~IfdefCommand() override;

        [[nodiscard]]
        std::optional<PreprocessorToken>
        Execute(Preprocessor      &preprocessor,
                tokenizer::Token &&ident) const override;
    };
}// namespace jcc::preprocessor::commands

#endif//IFDEF_COMMAND_H
