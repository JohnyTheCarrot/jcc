#include "define_command.h"

#include <algorithm>// for copy, __move_fn, move
#include <iterator> // for back_insert_iterator
#include <utility>
#include <variant>// for get, holds_alternative
#include <vector> // for vector

#include "diagnostics/variants/tk_pp/illegal_macro_redef.hpp"
#include "diagnostics/variants/tk_pp/invalid_macro_param.hpp"
#include "diagnostics/variants/tk_pp/macro_ellipsis_not_last.hpp"
#include "diagnostics/variants/tk_pp/macro_expected_comma_or_rparen.hpp"
#include "diagnostics/variants/tk_pp/macro_name_not_ident.hpp"
#include "misc/Span.h"
#include "parsing/parser.h"
#include "preprocessor/commands/command.h"// for Command, CommandMap
#include "preprocessor/macro.h"
#include "preprocessor/macro_store.h"          // for MacroStore
#include "preprocessor/preprocessor.h"         // for Preprocessor
#include "preprocessor/preprocessor_iterator.h"// for PreprocessorIterator...
#include "tokenizer/token.h"                   // for Identifier, Token

namespace jcc::preprocessor::commands {
    macro::ReplacementList GatherReplacementList(Preprocessor &preprocessor) {
        macro::ReplacementList replacementList{};

        auto const currentPos{
                ++preprocessor.Current<PreprocessorIteratorVerbatim>()
        };
        auto const untilNewLine{
                preprocessor.Until<PreprocessorIteratorVerbatim>(
                        tokenizer::SpecialPurpose::NewLine
                )
        };

        std::copy(
                currentPos, untilNewLine,
                std::back_inserter(replacementList.m_ReplacementList)
        );

        return replacementList;
    }

    std::pair<bool, macro::FunctionLikeMacro::ParameterList>
    GatherParameterList(Preprocessor &preprocessor, Span &span) {
        macro::FunctionLikeMacro::ParameterList parameterList{};

        auto token{pp_token::GetBasicToken(
                preprocessor.GetNextFromTokenizer(false, false)
        )};

        while (true) {
            if (!std::holds_alternative<tokenizer::Identifier>(token.m_Value)) {
                if (!token.Is(tokenizer::Punctuator::Ellipsis)) {
                    parsing::CompilerState::GetInstance()
                            .EmplaceFatalDiagnostic<
                                    diagnostics::InvalidMacroParam>(
                                    span.m_Source, span.m_Span,
                                    token.m_Span.m_Span
                            );
                }

                token = pp_token::GetBasicToken(
                        preprocessor.GetNextPreprocessorToken()
                );
                if (!token.Is(tokenizer::Punctuator::RightParenthesis)) {
                    parsing::CompilerState::GetInstance()
                            .EmplaceFatalDiagnostic<
                                    diagnostics::MacroEllipsisNotLast>(
                                    span.m_Source, span.m_Span,
                                    token.m_Span.m_Span
                            );
                }

                return std::make_pair(true, parameterList);
            }
            auto &identifier{std::get<tokenizer::Identifier>(token.m_Value)};
            parameterList.push_back(std::move(identifier));

            token = pp_token::GetBasicToken(
                    preprocessor.GetNextPreprocessorToken()
            );
            if (!std::holds_alternative<tokenizer::Punctuator>(token.m_Value)) {
                parsing::CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<
                                diagnostics::MacroExpectedCommaOrRparen>(
                                span.m_Source, span.m_Span, token.m_Span.m_Span
                        );
            }

            auto const punctuator{std::get<tokenizer::Punctuator>(token.m_Value)
            };

            if (punctuator == tokenizer::Punctuator::RightParenthesis) {
                return std::make_pair(false, parameterList);
            }

            if (punctuator != tokenizer::Punctuator::Comma) {
                parsing::CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<
                                diagnostics::MacroExpectedCommaOrRparen>(
                                span.m_Source, span.m_Span, token.m_Span.m_Span
                        );
            }

            token = pp_token::GetBasicToken(
                    preprocessor.GetNextPreprocessorToken(false, false)
            );
        }
    }

    void DefineCommand::DefineObjectLikeMacro(
            Preprocessor &preprocessor, Span &&span, std::string &&name,
            tokenizer::Token &&firstToken
    ) {
        macro::ReplacementList replacementList{
                {BasicPreprocessorToken{std::move(firstToken)}}
        };
        auto [m_ReplacementList]{GatherReplacementList(preprocessor)};
        std::ranges::move(
                m_ReplacementList,
                std::back_inserter(replacementList.m_ReplacementList)
        );

        preprocessor.GetMacroStore().RegisterMacro(
                name,
                macro::ObjectLikeMacro{
                        name, std::move(span), std::move(replacementList)
                }
        );
    }

    void DefineCommand::DefineFunctionLikeMacro(
            Preprocessor &preprocessor, Span &&span, std::string &&name
    ) {
        auto [isVariadic, parameterList]{GatherParameterList(preprocessor, span)
        };
        auto [m_ReplacementList]{GatherReplacementList(preprocessor)};

        macro::FunctionLikeMacro macro{
                name,
                std::move(span),
                {std::move(m_ReplacementList)},
                std::move(parameterList),
                isVariadic
        };
        preprocessor.GetMacroStore().RegisterMacro(name, std::move(macro));
    }

    DefineCommand::DefineCommand(CommandMap &map)
        : Command(map, tokenizer::Directive::Define) {
    }

    DefineCommand::~DefineCommand() = default;

    std::optional<PreprocessorToken> DefineCommand::Execute(
            Preprocessor &preprocessor, tokenizer::Token &&directive
    ) const {
        auto nextToken{
                pp_token::GetBasicToken(preprocessor.SimpleTokenRead(false))
        };

        auto const isIdent{
                std::holds_alternative<tokenizer::Identifier>(nextToken.m_Value)
        };
        auto const isKeyword{
                std::holds_alternative<tokenizer::Keyword>(nextToken.m_Value)
        };

        if (!isIdent && !isKeyword) {
            parsing::CompilerState::GetInstance()
                    .EmplaceDiagnostic<diagnostics::MacroNameNotIdent>(
                            directive.m_Span.m_Source, directive.m_Span.m_Span,
                            nextToken.m_Span.m_Span
                    );
            preprocessor.SkipToNextLine();
            return std::nullopt;
        }

        auto        macroNameSpan{nextToken.m_Span};
        std::string macroName{
                isIdent ? std::get<tokenizer::Identifier>(nextToken.m_Value)
                                  .m_Name
                        : KeywordToString(
                                  std::get<tokenizer::Keyword>(nextToken.m_Value
                                  )
                          )
        };

        if (auto const *macro{preprocessor.GetMacroStore().GetMacro(macroName)};
            macro != nullptr) {
            auto &compilerState{parsing::CompilerState::GetInstance()};
            auto  macroSpan{std::visit(
                    [](auto const &macro) { return macro.m_Span; }, *macro
            )};

            compilerState.EmplaceDiagnostic<diagnostics::IllegalMacroRedef>(
                    std::move(macroSpan), std::move(macroNameSpan)
            );
            return std::nullopt;
        }

        auto nextPpToken{preprocessor.SimpleTokenRead(false)};
        nextToken = pp_token::GetToken(nextPpToken);

        if (nextToken.Is(tokenizer::SpecialPurpose::NewLine)) {
            preprocessor.GetMacroStore().RegisterMacro(
                    macroName,
                    macro::ObjectLikeMacro{
                            macroName, std::move(macroNameSpan), {}
                    }
            );
            return nextPpToken;
        }

        if (nextToken.Is(tokenizer::Punctuator::PpLeftParenthesis)) {
            DefineFunctionLikeMacro(
                    preprocessor, std::move(macroNameSpan), std::move(macroName)
            );
        } else {
            DefineObjectLikeMacro(
                    preprocessor, std::move(macroNameSpan),
                    std::move(macroName), std::move(nextToken)
            );
        }

        return std::nullopt;
    }
}// namespace jcc::preprocessor::commands
