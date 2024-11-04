#include "undef_command.h"

#include <utility>// for move
#include <variant>// for get

#include "misc/Diagnosis.h"               // for Diagnosis, FatalCompilerE...
#include "preprocessor/commands/command.h"// for Command, CommandMap
#include "preprocessor/macro_store.h"     // for MacroStore
#include "preprocessor/preprocessor.h"    // for Preprocessor
#include "tokenizer/token.h"              // for Identifier, Token, Directive

namespace jcc::preprocessor::commands {
    UndefCommand::UndefCommand(CommandMap &map)
        : Command{map, tokenizer::Directive::Undef} {
    }

    UndefCommand::~UndefCommand() = default;

    std::optional<PreprocessorToken>
    UndefCommand::Execute(Preprocessor &preprocessor, tokenizer::Token &&)
            const {
        auto identToken{preprocessor.SimpleTokenRead()};

        if (!identToken.m_Token.Is<tokenizer::Identifier>()) {
            throw FatalCompilerError{
                    Diagnosis::Kind::PP_MacroExpectedIdentifier,
                    std::move(identToken.m_Token.m_Span)
            };
        }

        auto const ident{
                std::get<tokenizer::Identifier>(identToken.m_Token.m_Value)
                        .m_Name
        };
        preprocessor.GetMacroStore().UnregisterMacro(ident);

        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
