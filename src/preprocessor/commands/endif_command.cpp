#include "endif_command.h"

#include "preprocessor/preprocessor.h"

namespace jcc::preprocessor::commands {
    EndifCommand::EndifCommand(CommandMap &map)
        : Command{map, tokenizer::Directive::Endif} {
    }

    EndifCommand::~EndifCommand() = default;

    std::optional<PreprocessorToken>
    EndifCommand::Execute(Preprocessor &preprocessor, tokenizer::Token &&)
            const {
        preprocessor.PopConditional();

        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
