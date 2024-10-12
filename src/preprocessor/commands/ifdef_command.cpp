#include "ifdef_command.h"

#include <cassert>

#include "preprocessor/preprocessor.h"

namespace jcc::preprocessor::commands {
    void
    IfdefCommand::ExecuteCondition(bool isIfndef, Preprocessor &preprocessor) {
        auto ident{preprocessor.SimpleTokenRead()};
        if (!ident.m_Token.Is<tokenizer::Identifier>()) {
            throw FatalCompilerError{
                    Diagnosis::Kind::PP_CondExpectedIdentifier,
                    std::move(ident.m_Token.m_Span)
            };
        }
        auto const identStr{
                std::get<tokenizer::Identifier>(ident.m_Token.m_Value).m_Name
        };

        auto const isDefined{
                preprocessor.GetMacroStore().IsMacroDefined(identStr)
        };

        if (isDefined == !isIfndef) {
            return;
        }

        auto const conditionEnd{preprocessor.SkipUntilConditionEnd()};

        auto const directive{std::get<tokenizer::Directive>(conditionEnd.m_Value
        )};

        switch (directive) {
            case tokenizer::Directive::Endif:
                preprocessor.PopConditional();
                return;
            case tokenizer::Directive::Elifdef:
                ++preprocessor.Current<PreprocessorIteratorNoCommands>();
                ExecuteCondition(false, preprocessor);
                return;
            case tokenizer::Directive::Elifndef:
                ++preprocessor.Current<PreprocessorIteratorNoCommands>();
                ExecuteCondition(true, preprocessor);
                return;
            case tokenizer::Directive::Else:
                return;

            case tokenizer::Directive::If:
                throw FatalCompilerError{
                        Diagnosis::Kind::TODO, preprocessor.GetCurrentSpan()
                };
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

    std::optional<PreprocessorToken>
    IfdefCommand::Execute(Preprocessor &preprocessor, tokenizer::Token &&)
            const {
        preprocessor.PushConditional();

        ExecuteCondition(m_IsIfndef, preprocessor);
        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
