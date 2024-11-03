#include "include_command.h"

#include "preprocessor/preprocessor.h"

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
