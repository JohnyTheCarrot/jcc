#include "warning_command.h"

#include "preprocessor/preprocessor.h"

namespace jcc::preprocessor::commands {
    WarningCommand::WarningCommand(CommandMap &map)
        : Command(map, tokenizer::Directive::Warning) {
    }

    WarningCommand::~WarningCommand() = default;

    std::optional<PreprocessorToken> WarningCommand::Execute(
            Preprocessor &preprocessor, tokenizer::Token &&directive
    ) const {
        std::vector<tokenizer::Token> tokens;

        std::move(
                ++preprocessor.Current<PreprocessorIteratorNoCommands>(),
                preprocessor.Until<PreprocessorIteratorNoCommands>(
                        tokenizer::SpecialPurpose::NewLine
                ),
                std::back_inserter(tokens)
        );

        if (tokens.empty())
            throw jcc::FatalCompilerError{
                    Diagnosis::Kind::UnexpectedEOF, directive.m_Span
            };

        auto const [_, lastTokenSpan]{tokens.back()};
        std::stringstream ss;
        std::ranges::copy(
                tokens, std::ostream_iterator<tokenizer::Token>{ss, " "}
        );

        Diagnosis diag{
                directive.m_Span + lastTokenSpan, Diagnosis::Class::Warning,
                Diagnosis::Kind::PP_Custom, std::move(ss.str())
        };

        preprocessor.EmitWarning(std::move(diag));

        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
