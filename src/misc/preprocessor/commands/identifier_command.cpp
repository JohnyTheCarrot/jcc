#include "identifier_command.h"
#include "../preprocessor.h"

namespace jcc::preprocessor::commands {
	macro::FnMacroArguments
	IdentifierCommand::GatherArguments(Preprocessor &preprocessor, macro::FunctionLikeMacro const &fnMacro) {
		auto const [value, span]{preprocessor()};
		if (!std::holds_alternative<Tokenizer::Punctuator>(value))
			throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedLParen, span};

		if (auto const punctuator{std::get<Tokenizer::Punctuator>(value)};
		    punctuator != Tokenizer::Punctuator::LeftParenthesis &&
		    punctuator != Tokenizer::Punctuator::PpLeftParenthesis)
			throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedLParen, span};

		macro::FnMacroArguments arguments{};
		while (true) {
			auto [hasNext, argumentTokens]{GatherArgumentTokens(preprocessor, fnMacro)};

			std::string currentArgName;
			if (arguments.size() < fnMacro.m_ParameterList.size())
				currentArgName = fnMacro.m_ParameterList[arguments.size()].m_Name;
			else if (fnMacro.m_IsVA)
				currentArgName = std::string{VaArgs};
			else
				throw FatalCompilerError{Diagnosis::Kind::PP_MacroTooManyArgs, span};

			arguments.emplace(currentArgName, std::move(argumentTokens));

			if (!hasNext)
				break;
		}

		// if we've got variadic arguments AND the macro is not variadic, it means we've got too many arguments
		if (auto const vaArgsIter{arguments.find(std::string{VaArgs})};
		    vaArgsIter != arguments.end() && !fnMacro.m_IsVA)
			throw FatalCompilerError{Diagnosis::Kind::PP_MacroTooManyArgs, span};

		if (arguments.size() < fnMacro.m_ParameterList.size())
			throw FatalCompilerError{Diagnosis::Kind::PP_MacroTooFewArgs, span};

		return arguments;
	}

	std::pair<bool, std::vector<Tokenizer::Token>>
	IdentifierCommand::GatherArgumentTokens(Preprocessor &preprocessor, macro::FunctionLikeMacro const &fnMacro) {
		std::vector<Tokenizer::Token> argumentTokens{};
		int                           numLeftParentheses{1};

		for (auto token: preprocessor) {
			if (std::holds_alternative<Tokenizer::Punctuator>(token.m_Value)) {
				switch (std::get<Tokenizer::Punctuator>(token.m_Value)) {
					case Tokenizer::Punctuator::LeftParenthesis:
					case Tokenizer::Punctuator::PpLeftParenthesis:
						++numLeftParentheses;
						break;
					case Tokenizer::Punctuator::RightParenthesis:
						--numLeftParentheses;
						if (numLeftParentheses == 0)
							return {false, argumentTokens};
						break;
					case Tokenizer::Punctuator::Comma:
						if (numLeftParentheses == 1)
							return {true, argumentTokens};
						break;
					default:
						break;
				}
			}

			argumentTokens.emplace_back(std::move(token));
		}

		throw FatalCompilerError{Diagnosis::Kind::UnexpectedEOF, preprocessor.GetCurrentSpan()};
	}

	IdentifierCommand::IdentifierCommand(CommandMap &map)
	    : Command{map, Tokenizer::Token::GenericType::Identifier} {
	}

	IdentifierCommand::~IdentifierCommand() = default;

	std::optional<Tokenizer::Token>
	IdentifierCommand::Execute(Preprocessor &preprocessor, Tokenizer::Token &&ident) const {
		auto identifierContent{std::get<Tokenizer::Identifier>(ident.m_Value).m_Name};

		if (auto const macro{preprocessor.GetMacro(identifierContent)}) {
			auto macroInvocation{[macro, &identifierContent, &preprocessor] {
				if (std::holds_alternative<macro::FunctionLikeMacro>(*macro)) {
					macro::FnMacroArguments arguments{
					        GatherArguments(preprocessor, std::get<macro::FunctionLikeMacro>(*macro))
					};

					return macro::MacroInvocation{.m_MacroName = identifierContent, .m_Args = std::move(arguments)};
				}

				return macro::MacroInvocation{.m_MacroName = identifierContent};
			}()};

			preprocessor.InvokeMacro(std::move(macroInvocation));

			return preprocessor();// return the first token of the macro
		}

		if (auto arg{preprocessor.GetMacroArgument(identifierContent)}; arg.has_value()) {
			preprocessor.UseMacroArguments(std::move(arg.value()));
			return preprocessor();
		}

		return ident;
	}
}// namespace jcc::preprocessor::commands
