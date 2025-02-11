#include "else_command.h"

#include <variant>// for get

#include "diagnostics/variants/tk_pp/pp_conditional_not_terminated.hpp"
#include "misc/Diagnosis.h"// for Diagnosis, FatalCompilerE...
#include "parsing_sema/parser.h"
#include "preprocessor/commands/command.h"// for Command, CommandMap
#include "preprocessor/preprocessor.h"    // for Preprocessor
#include "tokenizer/token.h"              // for Token, Directive

namespace jcc::preprocessor::commands {
    ElseCommand::ElseCommand(CommandMap &map)
        : Command{map, tokenizer::Directive::Else} {
    }

    ElseCommand::~ElseCommand() = default;

    std::optional<PreprocessorToken> ElseCommand::Execute(
            Preprocessor &preprocessor, tokenizer::Token &&token
    ) const {
        // if we reached this point, we know that the previous condition was met, and we must skip the next group
        auto const conditionEnd{preprocessor.SkipUntilConditionEnd()};

        if (!conditionEnd.has_value()) {
            auto &compilerState{parsing_sema::CompilerState::GetInstance()};
            compilerState.EmplaceFatalDiagnostic<
                    diagnostics::PpConditionalNotTerminated>(
                    token.m_Span.m_Source, token.m_Span.m_Span,
                    preprocessor.GetCurrentSpan().m_Span
            );
        }

        auto const directive{
                std::get<tokenizer::Directive>(conditionEnd->m_Value)
        };

        switch (directive) {
            case tokenizer::Directive::Endif:
                return std::nullopt;
            default:
                // TODO: diagnosis
                throw FatalCompilerError{
                        // Diagnosis::Kind::PP_ExpectedEndif,
                        // preprocessor.GetCurrentSpan()
                };
        }
    }
}// namespace jcc::preprocessor::commands
