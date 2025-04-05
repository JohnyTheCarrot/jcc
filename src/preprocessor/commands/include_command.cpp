#include "include_command.h"

#include <string>// for char_traits
#include <utility>
#include <variant>// for get

#include "diagnostics/variants/tk_pp/directive_expected_newline.hpp"
#include "diagnostics/variants/tk_pp/include_expected_header_name.hpp"
#include "diagnostics/variants/tk_pp/include_open_failed.hpp"
#include "misc/Span.h"
#include "mjolnir/span.hpp"
#include "parsing/parser.h"
#include "preprocessor/commands/command.h"// for Command, CommandMap
#include "preprocessor/preprocessor.h"    // for Preprocessor
#include "tokenizer/token.h"              // for StringConstant, Token
#include "tokenizer/tokenizer.h"

namespace jcc::preprocessor::commands {
    IncludeCommand::IncludeCommand(CommandMap &map)
        : Command(map, tokenizer::Directive::Include) {
    }

    IncludeCommand::~IncludeCommand() = default;

    std::optional<PreprocessorToken> IncludeCommand::Execute(
            Preprocessor &preprocessor, tokenizer::Token &&directiveToken
    ) const {
        // TODO: #include <header>, there is no support for this yet.
        auto const nextToken{
                pp_token::GetBasicToken(preprocessor.SimpleTokenRead())
        };

        if (!nextToken.Is<tokenizer::StringConstant>()) {
            parsing::CompilerState::GetInstance()
                    .EmplaceDiagnostic<diagnostics::IncludeExpectedHeaderName>(
                            directiveToken.m_Span.m_Source,
                            directiveToken.m_Span.m_Span,
                            nextToken.m_Span.m_Span
                    );
            preprocessor.SkipToNextLine();
            return std::nullopt;
        }

        if (auto const newlineToken{
                    pp_token::GetBasicToken(preprocessor.SimpleTokenRead())
            };
            !newlineToken.Is(tokenizer::SpecialPurpose::NewLine)) {
            parsing::CompilerState::GetInstance()
                    .EmplaceDiagnostic<diagnostics::DirectiveExpectedNewline>(
                            directiveToken.m_Span.m_Source,
                            directiveToken.m_Span.m_Span +
                                    nextToken.m_Span.m_Span,
                            newlineToken.m_Span.m_Span
                    );
            preprocessor.SkipToNextLine();
            return std::nullopt;
        }

        auto const &[headerFileName, _prefix]{
                std::get<tokenizer::StringConstant>(nextToken.m_Value)
        };

        try {
            preprocessor.OpenHeader(headerFileName);
        } catch (tokenizer::TokenizerFileOpenFailure const &) {
            parsing::CompilerState::GetInstance()
                    .EmplaceDiagnostic<diagnostics::IncludeOpenFailed>(
                            directiveToken.m_Span.m_Source,
                            directiveToken.m_Span.m_Span,
                            nextToken.m_Span.m_Span
                    );
        }

        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
