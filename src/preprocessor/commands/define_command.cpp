#include "define_command.h"
#include "preprocessor//preprocessor.h"

namespace jcc::preprocessor::commands {
	macro::ReplacementList DefineCommand::GatherReplacementList(Preprocessor &preprocessor) {
		macro::ReplacementList replacementList{};

		auto const currentPos{++preprocessor.Current<PreprocessorIteratorNoCommands>()};
		auto const untilNewLine{preprocessor.Until<PreprocessorIteratorNoCommands>(tokenizer::SpecialPurpose::NewLine)};

		std::copy(currentPos, untilNewLine, std::back_inserter(replacementList.m_ReplacementList));

		return replacementList;
	}

	std::pair<bool, macro::FunctionLikeMacro::ParameterList>
	DefineCommand::GatherParameterList(Preprocessor &preprocessor) {
		macro::FunctionLikeMacro::ParameterList parameterList{};

		auto [token, isFromMacro]{preprocessor.GetNextFromTokenizer(false)};

		while (true) {
			if (!std::holds_alternative<tokenizer::Identifier>(token.m_Value)) {
				if (!token.Is(tokenizer::Punctuator::Ellipsis))
					throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedIdentifier, token.m_Span};

				token = preprocessor.GetNextPreprocessorToken().m_Token;
				if (!token.Is(tokenizer::Punctuator::RightParenthesis)) {
					throw FatalCompilerError{Diagnosis::Kind::PP_MacroEllipsisNotLast, token.m_Span};
				}

				return std::make_pair(true, parameterList);
			}
			auto &identifier{std::get<tokenizer::Identifier>(token.m_Value)};
			parameterList.push_back(std::move(identifier));

			token = preprocessor.GetNextPreprocessorToken().m_Token;
			if (!std::holds_alternative<tokenizer::Punctuator>(token.m_Value)) {
				throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedCommaOrRParen, token.m_Span};
			}

			auto const punctuator{std::get<tokenizer::Punctuator>(token.m_Value)};

			if (punctuator == tokenizer::Punctuator::RightParenthesis) {
				return std::make_pair(false, parameterList);
			}

			if (punctuator != tokenizer::Punctuator::Comma) {
				throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedCommaOrRParen, token.m_Span};
			}

			token = preprocessor.GetNextPreprocessorToken().m_Token;
		}
	}

	void DefineCommand::DefineObjectLikeMacro(
	        Preprocessor &preprocessor, std::string &&name, tokenizer::Token &&firstToken
	) {
		macro::ReplacementList replacementList{{std::move(firstToken)}};
		auto [m_ReplacementList]{GatherReplacementList(preprocessor)};
		std::ranges::move(m_ReplacementList, std::back_inserter(replacementList.m_ReplacementList));

		preprocessor.RegisterMacro(name, macro::ObjectLikeMacro{name, std::move(replacementList)});
	}

	void DefineCommand::DefineFunctionLikeMacro(Preprocessor &preprocessor, std::string &&name) {
		auto [isVariadic, parameterList]{GatherParameterList(preprocessor)};
		auto [m_ReplacementList]{GatherReplacementList(preprocessor)};

		macro::FunctionLikeMacro macro{name, std::move(m_ReplacementList), std::move(parameterList), isVariadic};
		preprocessor.RegisterMacro(name, std::move(macro));
	}

	DefineCommand::DefineCommand(CommandMap &map)
	    : Command(map, tokenizer::Directive::Define) {
	}

	DefineCommand::~DefineCommand() = default;

	std::optional<PreprocessorToken> DefineCommand::Execute(Preprocessor &preprocessor, tokenizer::Token &&) const {
		auto nextToken{preprocessor.GetNextPreprocessorToken().m_Token};

		if (nextToken.Is(tokenizer::SpecialPurpose::Error)) {
			throw FatalCompilerError{Diagnosis::Kind::UnexpectedEOF, nextToken.m_Span};
		}

		auto const isIdent{std::holds_alternative<tokenizer::Identifier>(nextToken.m_Value)};
		auto const isKeyword{std::holds_alternative<tokenizer::Keyword>(nextToken.m_Value)};

		if (!isIdent && !isKeyword) {
			throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedIdentifier, nextToken.m_Span};
		}

		std::string macroName{
		        isIdent ? std::get<tokenizer::Identifier>(nextToken.m_Value).m_Name
		                : KeywordAsIdentString(std::get<tokenizer::Keyword>(nextToken.m_Value))
		};

		nextToken = preprocessor.GetNextPreprocessorToken().m_Token;

		if (nextToken.Is(tokenizer::Punctuator::PpLeftParenthesis)) {
			DefineFunctionLikeMacro(preprocessor, std::move(macroName));
		} else {
			DefineObjectLikeMacro(preprocessor, std::move(macroName), std::move(nextToken));
		}

		return std::nullopt;
	}
}// namespace jcc::preprocessor::commands
