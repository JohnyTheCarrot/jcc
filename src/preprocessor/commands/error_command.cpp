#include "error_command.h"

#include "preprocessor/preprocessor.h"

namespace jcc::preprocessor::commands {
    ErrorCommand::ErrorCommand(CommandMap &map)
        : Command(map, tokenizer::Directive::Error) {
    }

    ErrorCommand::~ErrorCommand() = default;

    std::optional<PreprocessorToken> ErrorCommand::Execute(
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

        throw FatalCompilerError{
                Diagnosis::Kind::PP_Custom, directive.m_Span + lastTokenSpan,
                std::move(ss.str())
        };
    }
}// namespace jcc::preprocessor::commands
