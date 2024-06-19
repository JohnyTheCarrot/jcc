#include "command.h"
#include "preprocessor.h"

namespace jcc::preprocessor {
	macro::ReplacementList DefineCommand::GatherReplacementList(Preprocessor &preprocessor) {
		macro::ReplacementList replacementList{};

		std::copy(
		        preprocessor.Current<InternalPreprocessorIterator>(),
		        preprocessor.Until<InternalPreprocessorIterator>(Tokenizer::SpecialPurpose::NewLine),
		        std::back_inserter(replacementList.m_ReplacementList)
		);

		return replacementList;
	}

	std::pair<bool, macro::FunctionLikeMacro::ParameterList>
	DefineCommand::GatherParameterList(Preprocessor &preprocessor) {
		macro::FunctionLikeMacro::ParameterList parameterList{};

		Tokenizer::Token token{preprocessor.GetNextFromTokenizer(false)};

		while (true) {
			if (!std::holds_alternative<Tokenizer::Identifier>(token.m_Value)) {
				if (!token.IsPunctuatorKind(Tokenizer::Punctuator::Ellipsis))
					throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedIdentifier, token.m_Span};

				token = preprocessor();
				if (!token.IsPunctuatorKind(Tokenizer::Punctuator::RightParenthesis)) {
					throw FatalCompilerError{Diagnosis::Kind::PP_MacroEllipsisNotLast, token.m_Span};
				}

				return std::make_pair(true, parameterList);
			}
			auto &identifier{std::get<Tokenizer::Identifier>(token.m_Value)};
			parameterList.push_back(std::move(identifier));

			token = preprocessor();
			if (!std::holds_alternative<Tokenizer::Punctuator>(token.m_Value)) {
				throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedCommaOrRParen, token.m_Span};
			}

			auto const punctuator{std::get<Tokenizer::Punctuator>(token.m_Value)};

			if (punctuator == Tokenizer::Punctuator::RightParenthesis) {
				return std::make_pair(false, parameterList);
			}

			if (punctuator != Tokenizer::Punctuator::Comma) {
				throw FatalCompilerError{Diagnosis::Kind::PP_MacroExpectedCommaOrRParen, token.m_Span};
			}

			token = preprocessor();
		}
	}

	void DefineCommand::DefineObjectLikeMacro(
	        Preprocessor &preprocessor, std::string &&name, Tokenizer::Token &&firstToken
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
	    : Command(map, Tokenizer::Directive::Define) {
	}

	std::optional<Tokenizer::Token> DefineCommand::Execute(Preprocessor &preprocessor) const {
		auto nextToken{preprocessor()};

		if (nextToken.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::Error)) {
			preprocessor.Diagnose(Diagnosis::Class::Error, Diagnosis::Kind::UnexpectedEOF, nextToken.m_Span);
			return nextToken;
		}

		auto const isIdent{std::holds_alternative<Tokenizer::Identifier>(nextToken.m_Value)};
		auto const isKeyword{std::holds_alternative<Tokenizer::Keyword>(nextToken.m_Value)};

		if (!isIdent && !isKeyword) {
			preprocessor.Diagnose(
			        Diagnosis::Class::Error, Diagnosis::Kind::PP_MacroExpectedIdentifier, nextToken.m_Span
			);
			return Tokenizer::Token{Tokenizer::SpecialPurpose::Error, nextToken.m_Span};
		}

		std::string macroName{
		        isIdent ? std::get<Tokenizer::Identifier>(nextToken.m_Value).m_Name
		                : Tokenizer::KeywordAsIdentString(std::get<Tokenizer::Keyword>(nextToken.m_Value))
		};

		nextToken = preprocessor();

		if (nextToken.IsPunctuatorKind(Tokenizer::Punctuator::PpLeftParenthesis)) {
			DefineFunctionLikeMacro(preprocessor, std::move(macroName));
		} else {
			DefineObjectLikeMacro(preprocessor, std::move(macroName), std::move(nextToken));
		}

		return std::nullopt;
	}

	PreprocessorCommandSingleton &PreprocessorCommandSingleton::GetInstance() {
		static PreprocessorCommandSingleton instance{};
		return instance;
	}
}// namespace jcc::preprocessor
