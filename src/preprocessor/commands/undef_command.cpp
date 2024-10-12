#include "undef_command.h"

#include "preprocessor/preprocessor.h"

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
