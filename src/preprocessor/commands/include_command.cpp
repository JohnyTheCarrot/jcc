#include "include_command.h"

#include <string> // for char_traits
#include <variant>// for get

#include "misc/Diagnosis.h"               // for Diagnosis, FatalCompilerE...
#include "preprocessor/commands/command.h"// for Command, CommandMap
#include "preprocessor/preprocessor.h"    // for Preprocessor
#include "tokenizer/token.h"              // for StringConstant, Token

namespace jcc::preprocessor::commands {
    IncludeCommand::IncludeCommand(CommandMap &map)
        : Command(map, tokenizer::Directive::Include) {
    }

    IncludeCommand::~IncludeCommand() = default;

    std::optional<PreprocessorToken>
    IncludeCommand::Execute(Preprocessor &preprocessor, tokenizer::Token &&)
            const {
        // TODO: #include <header>, there is no support for this yet.
        auto const nextToken{preprocessor.SimpleTokenRead().m_Token};

        if (!nextToken.Is<tokenizer::StringConstant>()) {
            throw FatalCompilerError{
                    Diagnosis::Kind::PP_InclDirectiveExpectedHeaderName,
                    nextToken.m_Span
            };
        }

        if (auto const newlineToken{preprocessor.SimpleTokenRead().m_Token};
            !newlineToken.Is(tokenizer::SpecialPurpose::NewLine)) {
            throw FatalCompilerError{
                    Diagnosis::Kind::PP_DirectiveExpectedNewline,
                    newlineToken.m_Span
            };
        }

        auto const &headerName{
                std::get<tokenizer::StringConstant>(nextToken.m_Value)
        };

        preprocessor.OpenHeader(headerName.m_String);

        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
