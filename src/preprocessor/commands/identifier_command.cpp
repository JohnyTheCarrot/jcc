#include "identifier_command.h"

#include <string>

#include "preprocessor/preprocessor.h"

namespace jcc::preprocessor::commands {
    macro::FnMacroArguments IdentifierCommand::GatherArguments(
            Preprocessor &preprocessor, macro::FunctionLikeMacro const &fnMacro
    ) {
        auto const [value, span]{preprocessor.GetNextPreprocessorToken().m_Token
        };
        if (!std::holds_alternative<tokenizer::Punctuator>(value))
            throw FatalCompilerError{
                    Diagnosis::Kind::PP_MacroExpectedLParen, span
            };

        if (auto const punctuator{std::get<tokenizer::Punctuator>(value)};
            punctuator != tokenizer::Punctuator::LeftParenthesis &&
            punctuator != tokenizer::Punctuator::PpLeftParenthesis)
            throw FatalCompilerError{
                    Diagnosis::Kind::PP_MacroExpectedLParen, span
            };

        macro::FnMacroArguments arguments{};
        auto const              collectVaArgs{[&] {
            auto const startIt{++preprocessor.Current()};
            auto const endIt{
                    preprocessor.Until(tokenizer::Punctuator::RightParenthesis)
            };
            std::string const vaArgsStr{VaArgs};

            auto const vaArgIt{arguments.emplace(
                    vaArgsStr, std::vector<tokenizer::Token>{}
            )};// add empty variadic arguments
            std::move(
                    startIt, endIt, std::back_inserter(vaArgIt.first->second)
            );
        }};

        if (fnMacro.m_ParameterList.empty() && fnMacro.m_IsVA) {
            collectVaArgs();
        } else {
            while (true) {
                auto [hasNext,
                      argumentTokens]{GatherArgumentTokens(preprocessor)};

                if (arguments.size() < fnMacro.m_ParameterList.size()) {
                    std::string currentArgName{
                            fnMacro.m_ParameterList[arguments.size()].m_Name
                    };
                    arguments.emplace(
                            currentArgName, std::move(argumentTokens)
                    );
                } else if (!fnMacro.m_ParameterList.empty() && !fnMacro.m_IsVA) {
                    throw FatalCompilerError{
                            Diagnosis::Kind::PP_MacroTooManyArgs, span
                    };
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
        if (vaArgsIter != arguments.end() && !fnMacro.m_IsVA)
            throw FatalCompilerError{
                    Diagnosis::Kind::PP_MacroTooManyArgs, span
            };

        if (vaArgsIter == arguments.end() && fnMacro.m_IsVA)
            arguments.emplace(
                    VaArgs, std::vector<tokenizer::Token>{}
            );// add empty variadic arguments

        if (arguments.size() < fnMacro.m_ParameterList.size())
            throw FatalCompilerError{Diagnosis::Kind::PP_MacroTooFewArgs, span};

        return arguments;
    }

    std::pair<bool, std::vector<tokenizer::Token>>
    IdentifierCommand::GatherArgumentTokens(Preprocessor &preprocessor) {
        std::vector<tokenizer::Token> argumentTokens{};
        int                           numLeftParentheses{1};
        auto const                    ppItStart{
                ++preprocessor.Current<InternalPreprocessorIterator>()
        };
        auto const ppItEnd{preprocessor.EndOfFile<InternalPreprocessorIterator>(
        )};

        for (auto &ppToken : std::ranges::subrange(ppItStart, ppItEnd)) {
            auto &[token, isFromMacro]{ppToken};

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
                        if (numLeftParentheses == 1)
                            return {true, argumentTokens};
                        break;
                    default:
                        break;
                }
            }

            argumentTokens.emplace_back(std::move(token));
        }

        throw FatalCompilerError{
                Diagnosis::Kind::UnexpectedEOF, preprocessor.GetCurrentSpan()
        };
    }

    IdentifierCommand::IdentifierCommand(CommandMap &map)
        : Command{map, tokenizer::Token::GenericType::Identifier} {
    }

    IdentifierCommand::~IdentifierCommand() = default;

    std::optional<PreprocessorToken> IdentifierCommand::Execute(
            Preprocessor &preprocessor, tokenizer::Token &&ident
    ) const {
        auto identifierContent{
                std::get<tokenizer::Identifier>(ident.m_Value).m_Name
        };

        if (auto const macro{
                    preprocessor.GetMacroStore().GetMacro(identifierContent)
            }) {
            auto macroInvocation{[macro, &identifierContent, &preprocessor] {
                if (std::holds_alternative<macro::FunctionLikeMacro>(*macro)) {
                    macro::FnMacroArguments arguments{GatherArguments(
                            preprocessor,
                            std::get<macro::FunctionLikeMacro>(*macro)
                    )};

                    return macro::MacroInvocation{
                            .m_MacroName = identifierContent,
                            .m_Args      = std::move(arguments)
                    };
                }

                return macro::MacroInvocation{.m_MacroName = identifierContent};
            }()};

            preprocessor.GetMacroStore().InvokeMacro(std::move(macroInvocation)
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
