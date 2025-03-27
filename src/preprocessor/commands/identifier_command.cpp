#include "identifier_command.h"

#include <algorithm>    // for move
#include <iterator>     // for back_insert_iterator
#include <ranges>       // for subrange
#include <string>       // for operator==, hash
#include <unordered_map>// for operator==, _Node_it...
#include <variant>      // for get, holds_alternative

#include "diagnostics/variants/tk_pp/macro_invoc_invalid_num_args.hpp"
#include "diagnostics/variants/unexpected_eof.hpp"
#include "parsing_sema/parser.h"
#include "preprocessor/commands/command.h"     // for Command, CommandMap
#include "preprocessor/macro_store.h"          // for MacroStore
#include "preprocessor/preprocessor.h"         // for Preprocessor, VaArgs
#include "preprocessor/preprocessor_iterator.h"// for PreprocessorIterator

namespace jcc::preprocessor::commands {
    std::pair<bool, std::vector<tokenizer::Token>>
    GatherArgumentTokens(Preprocessor &preprocessor, mjolnir::Span &span) {
        std::vector<tokenizer::Token> argumentTokens{};
        int                           numLeftParentheses{1};
        auto const                    ppItStart{
                ++preprocessor.Current<InternalPreprocessorIterator>()
        };
        auto const ppItEnd{preprocessor.EndOfFile<InternalPreprocessorIterator>(
        )};

        for (auto &[token, isFromMacro] :
             std::ranges::subrange(ppItStart, ppItEnd)) {
            if (!isFromMacro &&
                std::holds_alternative<tokenizer::Punctuator>(token.m_Value)) {
                switch (std::get<tokenizer::Punctuator>(token.m_Value)) {
                    case tokenizer::Punctuator::LeftParenthesis:
                    case tokenizer::Punctuator::PpLeftParenthesis:
                        ++numLeftParentheses;
                        break;
                    case tokenizer::Punctuator::RightParenthesis:
                        --numLeftParentheses;
                        if (numLeftParentheses == 0)
                            return {false, argumentTokens};
                        break;
                    case tokenizer::Punctuator::Comma:
                        if (numLeftParentheses == 1) {
                            span += token.m_Span.m_Span;

                            return {true, argumentTokens};
                        }
                        break;
                    default:
                        break;
                }
            }
            span += token.m_Span.m_Span;

            argumentTokens.emplace_back(std::move(token));
        }

        auto currentSpan{preprocessor.GetCurrentSpan()};
        parsing_sema::CompilerState::GetInstance()
                .EmplaceFatalDiagnostic<diagnostics::UnexpectedEof>(
                        std::move(currentSpan.m_Source), currentSpan.m_Span
                );
    }

    [[nodiscard]]
    bool IsLeftParenthesis(tokenizer::Token const &token) {
        auto const [value, span]{token};
        if (!std::holds_alternative<tokenizer::Punctuator>(value))
            return false;

        if (auto const punctuator{std::get<tokenizer::Punctuator>(value)};
            punctuator != tokenizer::Punctuator::LeftParenthesis &&
            punctuator != tokenizer::Punctuator::PpLeftParenthesis)
            return false;

        return true;
    }

    std::string const c_VaArgsStr{VaArgs};

    macro::FnMacroArguments GatherArguments(
            Preprocessor &preprocessor, macro::FunctionLikeMacro const &fnMacro,
            Span &span
    ) {
        mjolnir::Span argsSpan{span.m_Span.end(), span.m_Span.end() + 1};
        macro::FnMacroArguments arguments{};
        auto const              collectVaArgs{[&] {
            auto const startIt{++preprocessor.Current()};
            auto const endIt{
                    preprocessor.Until(tokenizer::Punctuator::RightParenthesis)
            };
            auto const [value, _]{arguments.emplace(
                    c_VaArgsStr, std::vector<tokenizer::Token>{}
            )};// add empty variadic arguments

            for (auto it{startIt}; it != endIt; ++it) {
                argsSpan += it->m_Span.m_Span;
                value->second.emplace_back(std::move(*it));
            }
        }};

        if (fnMacro.m_ParameterList.empty() && fnMacro.m_IsVA) {
            collectVaArgs();
        } else {
            while (true) {
                auto [hasNext, argumentTokens]{
                        GatherArgumentTokens(preprocessor, argsSpan)
                };

                if (arguments.size() < fnMacro.m_ParameterList.size()) {
                    std::string currentArgName{
                            fnMacro.m_ParameterList[arguments.size()].m_Name
                    };
                    arguments.emplace(
                            std::move(currentArgName), std::move(argumentTokens)
                    );
                } else if (!fnMacro.m_ParameterList.empty() &&
                           !fnMacro.m_IsVA) {
                    parsing_sema::CompilerState::GetInstance()
                            .EmplaceFatalDiagnostic<
                                    diagnostics::MacroInvocInvalidNumArgs>(
                                    span.m_Source, span.m_Span, argsSpan,
                                    fnMacro.m_ParameterList.size(),
                                    arguments.size() + 1, false
                            );
                }

                if (arguments.size() >= fnMacro.m_ParameterList.size() &&
                    fnMacro.m_IsVA) {
                    collectVaArgs();
                    break;
                }

                if (!hasNext)
                    break;
            }
        }
        auto const vaArgsIter{arguments.find(std::string{VaArgs})};
        // if we've got variadic arguments AND the macro is not variadic, it means we've got too many arguments
        if (vaArgsIter != arguments.end() && !fnMacro.m_IsVA) {
            parsing_sema::CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<
                            diagnostics::MacroInvocInvalidNumArgs>(
                            span.m_Source, span.m_Span, argsSpan,
                            fnMacro.m_ParameterList.size(), arguments.size(),
                            false
                    );
        }

        if (vaArgsIter == arguments.end() && fnMacro.m_IsVA)
            arguments.emplace(
                    VaArgs, std::vector<tokenizer::Token>{}
            );// add empty variadic arguments

        // -1 if the macro is variadic, 0 otherwise
        if (arguments.size() - fnMacro.m_IsVA <
            fnMacro.m_ParameterList.size()) {
            parsing_sema::CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<
                            diagnostics::MacroInvocInvalidNumArgs>(
                            std::move(span.m_Source), span.m_Span, argsSpan,
                            fnMacro.m_ParameterList.size(),
                            arguments.size() - fnMacro.m_IsVA, fnMacro.m_IsVA
                    );
        }

        return arguments;
    }

    IdentifierCommand::IdentifierCommand(CommandMap &map)
        : Command{map, tokenizer::Token::GenericType::Identifier} {
    }

    IdentifierCommand::~IdentifierCommand() = default;

    std::optional<PreprocessorToken> IdentifierCommand::Execute(
            Preprocessor &preprocessor, tokenizer::Token &&ident
    ) const {
        Span span{ident.m_Span};
        auto identifierContent{
                std::get<tokenizer::Identifier>(ident.m_Value).m_Name
        };

        if (auto const macro{
                    preprocessor.GetMacroStore().GetMacro(identifierContent)
            }) {
            if (std::holds_alternative<macro::FunctionLikeMacro>(*macro)) {
                auto token{preprocessor.GetNextPreprocessorToken()};
                if (!IsLeftParenthesis(token.m_Token)) {
                    return token;
                }

                span.m_Span += token.m_Token.m_Span.m_Span;
            }

            auto macroInvocation{[&] {
                if (std::holds_alternative<macro::FunctionLikeMacro>(*macro)) {
                    macro::FnMacroArguments arguments{GatherArguments(
                            preprocessor,
                            std::get<macro::FunctionLikeMacro>(*macro), span
                    )};

                    return macro::MacroInvocation{
                            .m_MacroName = identifierContent,
                            .m_Args      = std::move(arguments)
                    };
                }

                return macro::MacroInvocation{.m_MacroName = identifierContent};
            }()};

            preprocessor.GetMacroStore().InvokeMacro(
                    std::move(macroInvocation)
            );

            return preprocessor.GetNextPreprocessorToken(
            );// return the first token of the macro
        }

        if (auto arg{preprocessor.GetMacroStore().GetMacroArgument(
                    identifierContent
            )};
            arg.has_value()) {
            preprocessor.GetMacroStore().PushMacroArgumentTokens(
                    std::move(arg.value())
            );
            return preprocessor.GetNextPreprocessorToken();
        }

        return PreprocessorToken{ident, false};
    }
}// namespace jcc::preprocessor::commands
