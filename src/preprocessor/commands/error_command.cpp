#include "error_command.h"

#include <algorithm>// for move, __copy_fn, copy
#include <iterator> // for back_insert_iterator
#include <sstream>  // for basic_stringstream
#include <utility>  // for move
#include <vector>   // for vector, allocator

#include "misc/Diagnosis.h"                    // for Diagnosis, FatalComp...
#include "misc/Span.h"                         // for Span
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
                ss.str()
        };
    }
}// namespace jcc::preprocessor::commands
