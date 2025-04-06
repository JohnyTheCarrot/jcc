#include "identifier_command.h"

#include <ranges>       // for subrange
#include <string>       // for operator==, hash
#include <unordered_map>// for operator==, _Node_it...
#include <utility>
#include <variant>// for get, holds_alternative
#include <vector>

#include "diagnostics/variants/tk_pp/macro_invoc_invalid_num_args.hpp"
#include "diagnostics/variants/unexpected_eof.hpp"
#include "misc/Span.h"
#include "mjolnir/span.hpp"
#include "parsing/parser.h"
#include "preprocessor/commands/command.h"// for Command, CommandMap
#include "preprocessor/macro.h"
#include "preprocessor/macro_store.h"          // for MacroStore
#include "preprocessor/preprocessor.h"         // for Preprocessor, VaArgs
#include "preprocessor/preprocessor_iterator.h"// for PreprocessorIterator
#include "tokenizer/token.h"

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

        for (auto &ppToken : std::ranges::subrange(ppItStart, ppItEnd)) {
            auto token{pp_token::GetToken(ppToken)};
            if (std::holds_alternative<tokenizer::Punctuator>(token.m_Value)) {
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
        parsing::CompilerState::GetInstance()
                .EmplaceFatalDiagnostic<diagnostics::UnexpectedEof>(
                        std::move(currentSpan.m_Source), currentSpan.m_Span
                );
    }

    [[nodiscard]]
    bool IsLeftParenthesis(tokenizer::Token const &token) {
        if (!std::holds_alternative<tokenizer::Punctuator>(token.m_Value))
            return false;

        if (auto const punctuator{std::get<tokenizer::Punctuator>(token.m_Value)
            };
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
            auto       parenDepth{1};
            auto const endIt{preprocessor.end()};
            auto const [value, _]{arguments.emplace(
                    c_VaArgsStr, std::vector<tokenizer::Token>{}
            )};// add empty variadic arguments

            for (auto it{startIt}; it != endIt; ++it) {
                argsSpan += it->m_Span.m_Span;
                if (IsLeftParenthesis(*it)) {
                    ++parenDepth;
                } else if (it->Is(tokenizer::Punctuator::RightParenthesis)) {
                    --parenDepth;
                    if (parenDepth == 0)
                        break;
                }
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
                    parsing::CompilerState::GetInstance()
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
            parsing::CompilerState::GetInstance()
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
            parsing::CompilerState::GetInstance()
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

        if (auto const *macro{
                    preprocessor.GetMacroStore().GetMacro(identifierContent)
            }) {
            if (std::holds_alternative<macro::FunctionLikeMacro>(*macro)) {
                // When a token is the right hand side of a glue operator, we can't
                // pop the macro stack to get the next token because that would violate
                // the macro expansion order. The glue operator should come first.
                //
                // Example:
                // ```
                // #define WORLD(x) hi
                // #define GLUE(x) HELLO_ ## x
                // #define TEST GLUE(WORLD)
                // #define HELLO_WORLD(x) (x + 1)
                // TEST(hi)
                // ```
                // If we allowed popping the macro stack for the right hand side of the
                // glue operator, then `x` would get replaced by `WORLD`, so far so good
                // but then since `WORLD` is an identifier, it will check if it's a
                // function-like macro by checking if the next token is a `(`.
                // Since `x` is the last token in GLUE's replacement list, it will pop
                // the macro stack to its parent macro to `TEST`, and then to the
                // invocation of TEST, which is followed by `(hi)`. This will cause
                // `WORLD` to be invoked before the glue takes place.
                //
                if (ident.m_IsGlueRhs) {
                    return BasicPreprocessorToken{std::move(ident)};
                }
                auto ppToken{preprocessor.GetNextPreprocessorToken()};
                auto token{pp_token::GetToken(ppToken)};
                if (!IsLeftParenthesis(token)) {
                    preprocessor.ReinsertToken(std::move(ppToken));
                    return BasicPreprocessorToken{std::move(ident)};
                }

                span.m_Span += token.m_Span.m_Span;
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

            auto next{preprocessor.GetNextPreprocessorToken(
                    true, true, ident.m_IsGlueRhs
            )};
            if (auto *basicPpToken{std::get_if<BasicPreprocessorToken>(&next)
                }) {
                basicPpToken->SetTokenGlueRhs(ident.m_IsGlueRhs);
            }
            return next;
        }

        if (auto arg{preprocessor.GetMacroStore().GetMacroArgument(
                    identifierContent
            )};
            arg.has_value()) {
            preprocessor.GetMacroStore().PushMacroArgumentTokens(
                    std::move(arg.value())
            );
            auto next{preprocessor.GetNextPreprocessorToken(
                    true, true, ident.m_IsGlueRhs
            )};
            if (auto *basicPpToken{std::get_if<BasicPreprocessorToken>(&next)
                }) {
                basicPpToken->SetTokenGlueRhs(ident.m_IsGlueRhs);
            }
            return next;
        }

        return BasicPreprocessorToken{std::move(ident)};
    }
}// namespace jcc::preprocessor::commands
