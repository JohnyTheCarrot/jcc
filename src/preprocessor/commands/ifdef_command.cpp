#include "ifdef_command.h"

#include <cassert>// for assert
#include <utility>// for move
#include <variant>// for get

#include "diagnostics/variants/tk_pp/pp_conditional_expected_ident.hpp"
#include "diagnostics/variants/tk_pp/pp_conditional_not_terminated.hpp"
#include "diagnostics/variants/todo.hpp"
#include "misc/Span.h"
#include "mjolnir/span.hpp"
#include "parsing/parser.h"
#include "preprocessor/commands/command.h"     // for Command, CommandMap
#include "preprocessor/macro_store.h"          // for MacroStore
#include "preprocessor/preprocessor.h"         // for Preprocessor
#include "preprocessor/preprocessor_iterator.h"// for PreprocessorIterator...
#include "tokenizer/token.h"                   // for Identifier, Directive

namespace jcc::preprocessor::commands {
    void IfdefCommand::ExecuteCondition(
            bool isIfndef, Preprocessor &preprocessor, Span const &span
    ) {
        auto ident{pp_token::GetBasicToken(preprocessor.SimpleTokenRead())};
        if (!ident.Is<tokenizer::Identifier>()) {
            auto      &compilerState{parsing::CompilerState::GetInstance()};
            auto const tokenSpan{[&] {
                auto const tkSpan{ident.m_Span.m_Span};

                return tkSpan.empty()
                             ? mjolnir::Span{tkSpan.start() - 1, tkSpan.start()}
                             : tkSpan;
            }()};

            compilerState.EmplaceFatalDiagnostic<
                    diagnostics::PpConditionalExpectedIdent>(
                    span.m_Source, span.m_Span, tokenSpan
            );
        }
        auto const identStr{
                std::get<tokenizer::Identifier>(ident.m_Value).m_Name
        };

        auto const isDefined{
                preprocessor.GetMacroStore().IsMacroDefined(identStr)
        };

        if (isDefined == !isIfndef) {
            return;
        }

        auto conditionEnd{preprocessor.SkipUntilConditionEnd()};

        if (!conditionEnd.has_value()) {
            auto &compilerState{parsing::CompilerState::GetInstance()};
            compilerState.EmplaceFatalDiagnostic<
                    diagnostics::PpConditionalNotTerminated>(
                    span.m_Source, span.m_Span,
                    preprocessor.GetCurrentSpan().m_Span
            );
        }

        auto const directive{
                std::get<tokenizer::Directive>(conditionEnd->m_Value)
        };

        switch (directive) {
            case tokenizer::Directive::Endif:
                preprocessor.PopConditional();
                return;
            case tokenizer::Directive::Elifdef:
                ++preprocessor.Current<PreprocessorIteratorNoCommands>();
                ExecuteCondition(false, preprocessor, span);
                return;
            case tokenizer::Directive::Elifndef:
                ++preprocessor.Current<PreprocessorIteratorNoCommands>();
                ExecuteCondition(true, preprocessor, span);
                return;
            case tokenizer::Directive::Else:
                return;

            case tokenizer::Directive::If:
                parsing::CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<diagnostics::TodoError>(
                                conditionEnd->m_Span.m_Source,
                                conditionEnd->m_Span.m_Span
                        );
            default:
                assert(false);
                return;
        }
    }

    IfdefCommand::IfdefCommand(CommandMap &map, bool isIfndef)
        : Command{map, isIfndef ? tokenizer::Directive::Ifndef
                                : tokenizer::Directive::Ifdef}
        , m_IsIfndef{isIfndef} {
    }

    IfdefCommand::~IfdefCommand() = default;

    std::optional<PreprocessorToken> IfdefCommand::Execute(
            Preprocessor &preprocessor, tokenizer::Token &&token
    ) const {
        preprocessor.PushConditional();

        ExecuteCondition(m_IsIfndef, preprocessor, token.m_Span);
        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
