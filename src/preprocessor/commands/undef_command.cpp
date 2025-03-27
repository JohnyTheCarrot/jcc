#include "undef_command.h"

#include <utility>// for move
#include <variant>// for get

#include "diagnostics/variants/tk_pp/undef_expected_ident.hpp"
#include "parsing_sema/parser.h"
#include "preprocessor/commands/command.h"// for Command, CommandMap
#include "preprocessor/macro_store.h"     // for MacroStore
#include "preprocessor/preprocessor.h"    // for Preprocessor
#include "tokenizer/token.h"              // for Identifier, Token, Directive

namespace jcc::preprocessor::commands {
    UndefCommand::UndefCommand(CommandMap &map)
        : Command{map, tokenizer::Directive::Undef} {
    }

    UndefCommand::~UndefCommand() = default;

    std::optional<PreprocessorToken> UndefCommand::Execute(
            Preprocessor &preprocessor, tokenizer::Token &&undefDirective
    ) const {
        auto const [token, isFromMacro]{preprocessor.SimpleTokenRead()};

        if (!token.Is<tokenizer::Identifier>()) {
            parsing_sema::CompilerState::GetInstance()
                    .EmplaceDiagnostic<diagnostics::UndefExpectedIdent>(
                            undefDirective.m_Span.m_Source,
                            undefDirective.m_Span.m_Span, token.m_Span.m_Span
                    );
            preprocessor.SkipToNextLine();
            return std::nullopt;
        }

        auto const ident{std::get<tokenizer::Identifier>(token.m_Value).m_Name};
        preprocessor.GetMacroStore().UnregisterMacro(ident);

        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
