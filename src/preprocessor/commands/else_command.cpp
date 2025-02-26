#include "else_command.h"

#include <variant>// for get

#include "diagnostics/variants/tk_pp/else_without_if.hpp"
#include "diagnostics/variants/tk_pp/pp_conditional_not_terminated.hpp"
#include "diagnostics/variants/tk_pp/unexpected_else.hpp"
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
        if (preprocessor.GetConditionalDepth() == 0) {
            parsing_sema::CompilerState::GetInstance()
                    .EmplaceDiagnostic<diagnostics::ElseWithoutIf>(
                            token.m_Span.m_Source, token.m_Span.m_Span
                    );
            return std::nullopt;
        }

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

        // The only directives that can be encountered here are conditional directives
        switch (directive) {
            case tokenizer::Directive::Endif:
                return std::nullopt;
            default:
                parsing_sema::CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<diagnostics::UnexpectedElse>(
                                token.m_Span.m_Source, token.m_Span.m_Span,
                                conditionEnd->m_Span.m_Span
                        );
        }
    }
}// namespace jcc::preprocessor::commands
