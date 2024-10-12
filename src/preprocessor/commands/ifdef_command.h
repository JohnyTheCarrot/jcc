#ifndef IFDEF_COMMAND_H
#define IFDEF_COMMAND_H
#include "command.h"

namespace jcc::preprocessor {
    class Preprocessor;
}

namespace jcc::preprocessor::commands {
    class IfdefCommand final : public Command {
        bool m_IsIfndef;

        static void ExecuteCondition(bool isIfndef, Preprocessor &preprocessor);

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
