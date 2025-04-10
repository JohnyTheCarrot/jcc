#include "error_command.h"

#include <algorithm>// for move, __copy_fn, copy
#include <iterator> // for back_insert_iterator
#include <sstream>  // for basic_stringstream
#include <utility>  // for move
#include <vector>   // for vector, allocator

#include "diagnostics/variants/tk_pp/custom_diagnostic.hpp"
#include "diagnostics/variants/tk_pp/diagnostic_directive_no_tokens.hpp"
#include "misc/Span.h"// for Span
#include "mjolnir/report.hpp"
#include "parsing/parser.h"
#include "preprocessor/commands/command.h"     // for Command, CommandMap
#include "preprocessor/preprocessor.h"         // for Preprocessor
#include "preprocessor/preprocessor_iterator.h"// for PreprocessorIterator...
#include "tokenizer/token.h"                   // for Token, operator<<

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

        auto &compilerState{parsing::CompilerState::GetInstance()};
        if (tokens.empty()) {
            compilerState.EmplaceDiagnostic<
                    diagnostics::DiagnosticDirectiveNoTokens>(
                    directive.m_Span.m_Source,
                    diagnostics::DiagnosticDirectiveKind::Error,
                    directive.m_Span.m_Span
            );

            return std::nullopt;
        }

        auto const [_, lastTokenSpan]{tokens.back()};
        std::stringstream ss;
        std::ranges::copy(
                tokens, std::ostream_iterator<tokenizer::Token>{ss, " "}
        );

        compilerState.EmplaceDiagnostic<diagnostics::CustomDiagnostic>(
                directive.m_Span.m_Source, directive.m_Span.m_Span, ss.str(),
                mjolnir::BasicReportKind::Error
        );

        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
