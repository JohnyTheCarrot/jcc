#include "else_command.h"

#include "preprocessor/preprocessor.h"

namespace jcc::preprocessor::commands {
    ElseCommand::ElseCommand(CommandMap &map)
        : Command{map, tokenizer::Directive::Else} {
    }

    ElseCommand::~ElseCommand() = default;

    std::optional<PreprocessorToken>
    ElseCommand::Execute(Preprocessor &preprocessor, tokenizer::Token &&)
            const {
        // if we reached this point, we know that the previous condition was met, and we must skip the next group
        auto const conditionEnd{preprocessor.SkipUntilConditionEnd()};

        auto const directive{std::get<tokenizer::Directive>(conditionEnd.m_Value
        )};

        switch (directive) {
            case tokenizer::Directive::Endif:
                return std::nullopt;
            default:
                throw FatalCompilerError{
                        Diagnosis::Kind::PP_ExpectedEndif,
                        preprocessor.GetCurrentSpan()
                };
        }
    }
}// namespace jcc::preprocessor::commands
