#include "Preprocessor.h"
#include <numeric>

namespace jcc {
	bool Preprocessor::MacroReplacementStackData::IsFinished() const noexcept {
		if (std::holds_alternative<ObjectLikeMacro>(m_Macro)) {
			return m_Progress == std::get<ObjectLikeMacro>(m_Macro).m_ReplacementList.m_ReplacementList.cend();
		}

		return m_Progress == std::get<FunctionLikeMacro>(m_Macro).m_ReplacementList.m_ReplacementList.cend();
	}

	Tokenizer::Identifier::IdentString const &Preprocessor::MacroReplacementStackData::GetMacroName() const noexcept {
		if (std::holds_alternative<FunctionLikeMacro>(m_Macro))
			return std::get<FunctionLikeMacro>(m_Macro).m_MacroName;

		return std::get<ObjectLikeMacro>(m_Macro).m_MacroName;
	}

	bool Preprocessor::ReplacementList::operator==(Preprocessor::ReplacementList const &other) const noexcept {
		if (m_ReplacementList.size() != other.m_ReplacementList.size())
			return false;

		//TODO: Strictly speaking, we need to check for whitespace equality. We could do so by utilising the spans.
		for (TokenList::const_iterator it{m_ReplacementList.cbegin()}, itOther{other.m_ReplacementList.cbegin()};
		     it != m_ReplacementList.cend(); ++it, ++itOther) {
			if (it->m_Value != itOther->m_Value)
				return false;
		}

		return true;
	}

	bool Preprocessor::ObjectLikeMacro::operator==(Preprocessor::ObjectLikeMacro const &other) const noexcept {
		return m_ReplacementList == other.m_ReplacementList;
	}

	bool Preprocessor::FunctionLikeMacro::operator==(Preprocessor::FunctionLikeMacro const &other) const noexcept {
		return m_ParameterList == other.m_ParameterList && m_IsVA == other.m_IsVA &&
		       m_ReplacementList == other.m_ReplacementList;
	}

	bool Preprocessor::ExecuteIncludeDirective(Tokenizer::Token const &includeDirectiveToken) {
		using HeaderType = Tokenizer::IncludeDirective::HeaderType;

		std::string fileName{};

		auto const directive{std::get<Tokenizer::IncludeDirective>(includeDirectiveToken.m_Value)};

		switch (directive.m_HeaderType) {
			case HeaderType::HChar:
			case HeaderType::MacroName:
				m_Diagnoses.emplace_back(
				        includeDirectiveToken.m_Span, Diagnosis::Class::Warning, Diagnosis::Kind::TODO
				);
				return true;
			case HeaderType::QChar:
				fileName = directive.m_Name;
				break;
		}

		auto const ifStream{m_IStreams.emplace_back(std::make_unique<std::ifstream>(fileName)).get()};

		if (!*ifStream) {
			m_Diagnoses.emplace_back(
			        includeDirectiveToken.m_Span, Diagnosis::Class::Error,
			        Diagnosis::Kind::PP_InclDirectiveFileOpenFailed
			);
			return false;
		}

		m_CurrentTokenizer =
		        m_TokenizerStack.emplace(std::make_unique<Tokenizer>(std::move(fileName), *ifStream, m_Diagnoses))
		                .get();
		return true;
	}

	bool Preprocessor::ExecuteMacroDefinition() {
		auto firstToken{GetNextToken()};

		if (firstToken.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::EndOfFile)) {
			m_Diagnoses.emplace_back(firstToken.m_Span, Diagnosis::Class::Error, Diagnosis::Kind::UnexpectedEOF);
			return false;
		}

		bool const isIdent{std::holds_alternative<Tokenizer::Identifier>(firstToken.m_Value)};

		if (!isIdent && !std::holds_alternative<Tokenizer::Keyword>(firstToken.m_Value)) {
			m_Diagnoses.emplace_back(
			        firstToken.m_Span, Diagnosis::Class::Error, Diagnosis::Kind::PP_MacroExpectedIdentifier
			);
			return false;
		}

		Tokenizer::Identifier::IdentString macroName{
		        isIdent ? std::get<Tokenizer::Identifier>(firstToken.m_Value).m_Name
		                : Tokenizer::KeywordAsIdentString(std::get<Tokenizer::Keyword>(firstToken.m_Value))
		};
		if (m_MacroDefinitions.find(macroName) != m_MacroDefinitions.end()) {
			m_Diagnoses.emplace_back(
			        firstToken.m_Span, Diagnosis::Class::Error, Diagnosis::Kind::PP_IllegalMacroRedefinition
			);
			return false;
		}

		auto                             replacementListToken{GetNextToken()};
		bool                             isFunctionLike{false};
		bool                             isVA{false};
		FunctionLikeMacro::ParameterList parameterList{};

		if (replacementListToken.IsPunctuatorKind(Tokenizer::Punctuator::PpLeftParenthesis)) {
			isFunctionLike = true;
			do {
				replacementListToken = GetNextToken();

				if (std::holds_alternative<Tokenizer::Identifier>(replacementListToken.m_Value)) {
					auto parameter{std::get<Tokenizer::Identifier>(replacementListToken.m_Value)};

					parameterList.emplace_back(std::move(parameter));
					replacementListToken = GetNextToken();
					continue;
				}

				bool const isEllipsis{replacementListToken.IsPunctuatorKind(Tokenizer::Punctuator::Ellipsis)};

				if (!isEllipsis) {
					m_Diagnoses.emplace_back(
					        replacementListToken.m_Span, Diagnosis::Class::Error,
					        Diagnosis::Kind::PP_IllegalMacroParameterToken
					);
					return false;
				}

				replacementListToken = GetNextToken();

				isVA = true;
				break;
			} while (replacementListToken.IsPunctuatorKind(Tokenizer::Punctuator::Comma));

			if (!replacementListToken.IsPunctuatorKind(Tokenizer::Punctuator::RightParenthesis)) {
				m_Diagnoses.emplace_back(
				        replacementListToken.m_Span, Diagnosis::Class::Error,
				        Diagnosis::Kind::PP_UnterminatedMacroParameterList
				);
				return false;
			}

			replacementListToken = GetNextToken();
		}

		ReplacementList replacementList{};

		while (true) {
			if (replacementListToken.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::Error)) {
				return false;
			}

			if (replacementListToken.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::EndOfFile) ||
			    replacementListToken.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::NewLine)) {
				break;
			}

			replacementList.m_ReplacementList.emplace_back(std::move(replacementListToken));
			replacementListToken = GetNextToken();
		}

		if (isFunctionLike) {
			FunctionLikeMacro macro{macroName, std::move(replacementList), std::move(parameterList), isVA};
			m_MacroDefinitions.insert({std::move(macroName), std::move(macro)});
			return true;
		}

		ObjectLikeMacro macro{macroName, std::move(replacementList)};
		m_MacroDefinitions.insert({std::move(macroName), std::move(macro)});
		return true;
	}

	Tokenizer::Token Preprocessor::GetNextToken() {
		while (!m_MacroStack.empty() && m_MacroStack.top().IsFinished()) {
			Tokenizer::Identifier::IdentString macroName{m_MacroStack.top().GetMacroName()};
			m_MacroStack.pop();

			if (macroName == VA_ARGS_MACRO_NAME) {
				continue;
			}

			if (m_MacroStack.empty()) {
				m_MacroDefinitions.erase(VA_ARGS_MACRO_NAME);
				break;
			}

			if (!std::holds_alternative<FunctionLikeMacro>(m_MacroStack.top().m_Macro) ||
			    !std::get<FunctionLikeMacro>(m_MacroStack.top().m_Macro).m_IsVA) {
				m_MacroDefinitions.erase(VA_ARGS_MACRO_NAME);
				continue;
			}

			ObjectLikeMacro const &vaMacro = m_MacroStack.top().m_VaArgsMacro.value();
			if (auto const vaArgsIt{m_MacroDefinitions.find(VA_ARGS_MACRO_NAME)}; vaArgsIt != m_MacroDefinitions.cend())
				m_MacroDefinitions.find(VA_ARGS_MACRO_NAME)->second = vaMacro;
			else
				m_MacroDefinitions.emplace(VA_ARGS_MACRO_NAME, vaMacro);
		}

		if (m_UnusedPeekResult.has_value()) {
			auto value{std::move(m_UnusedPeekResult.value())};
			m_UnusedPeekResult = std::nullopt;
			return value;
		}

		if (m_MacroStack.empty())
			return (*m_CurrentTokenizer)();

		MacroReplacementStackData &macroStackTop = m_MacroStack.top();

		if (macroStackTop.m_ArgumentTokenIter.has_value()) {
			if (macroStackTop.m_ArgumentTokenIter.value() != macroStackTop.m_ArgumentTokenIterEnd.value()) {
				return *(macroStackTop.m_ArgumentTokenIter.value()++);
			}

			macroStackTop.m_ArgumentTokenIter    = std::nullopt;
			macroStackTop.m_ArgumentTokenIterEnd = std::nullopt;
		}

		return *(macroStackTop.m_Progress++);
	}

	Tokenizer::Token Preprocessor::ExecuteHashOperator() {
		auto token{GetNextToken()};
		if (!std::holds_alternative<Tokenizer::Identifier>(token.m_Value)) {
			m_Diagnoses.emplace_back(
			        token.m_Span, Diagnosis::Class::Error, Diagnosis::Kind::PP_HashNotFollowedByParameter
			);
			return Tokenizer::Token{Tokenizer::SpecialPurpose::Error, token.m_Span};
		}

		auto const &parameterName{std::get<Tokenizer::Identifier>(token.m_Value).m_Name};

		MacroReplacementStackData const &replacementStackData = m_MacroStack.top();
		auto const                      &fnLikeMacro{std::get<FunctionLikeMacro>(replacementStackData.m_Macro)};
		auto const argumentIndex{FindParameterIndexByName(fnLikeMacro.m_ParameterList, parameterName)};
		bool const isVaArgsParam{
		        fnLikeMacro.m_IsVA && parameterName == VA_ARGS_MACRO_NAME &&
		        replacementStackData.m_VaArgsMacro.has_value()
		};

		if (!argumentIndex.has_value() && !isVaArgsParam) {
			m_Diagnoses.emplace_back(
			        token.m_Span, Diagnosis::Class::Error, Diagnosis::Kind::PP_HashNotFollowedByParameter
			);
			return Tokenizer::Token{Tokenizer::SpecialPurpose::Error, token.m_Span};
		}

		ReplacementList::TokenList const &replacementList{
		        isVaArgsParam ? replacementStackData.m_VaArgsMacro->m_ReplacementList.m_ReplacementList
		                      : replacementStackData.m_Arguments.value()[*argumentIndex].m_ReplacementList
		};
		std::optional<SpanMarker> lastTokenEndMarker{};

		std::string replacementListAsString{std::accumulate(
		        replacementList.cbegin(), replacementList.cend(), std::string{""},
		        [&](auto acc, auto const &token) {
			        auto str{std::move(acc)};

			        if (lastTokenEndMarker.has_value() &&
			            (lastTokenEndMarker->m_LineNumber != token.m_Span.m_Start.m_LineNumber ||
			             lastTokenEndMarker->m_CharacterIndex + 1 != token.m_Span.m_Start.m_CharacterIndex))
				        str += ' ';
			        lastTokenEndMarker = token.m_Span.m_End;

			        str += Tokenizer::TokenToString(token.m_Value);
			        return str;
		        }
		)};

		return Tokenizer::Token{Tokenizer::StringConstant{std::move(replacementListAsString)}, token.m_Span};
	}

	Tokenizer::Token Preprocessor::operator()() {
		while (true) {
			Tokenizer::Token token{GetNextToken()};

			if (std::holds_alternative<Tokenizer::SpecialPurpose>(token.m_Value)) {
				auto const specialPurpose{std::get<Tokenizer::SpecialPurpose>(token.m_Value)};

				switch (specialPurpose) {
					case Tokenizer::SpecialPurpose::EndOfFile:
						if (m_TokenizerStack.empty())
							return token;// eof

						m_TokenizerStack.pop();
						if (m_TokenizerStack.empty())
							m_CurrentTokenizer = &m_MainTokenizer;
						else
							m_CurrentTokenizer = m_TokenizerStack.top().get();
						continue;
					case Tokenizer::SpecialPurpose::NewLine:
						// the preprocessor may not return newline as a token
						continue;
					default:
						return token;
				}
			}

			if (std::holds_alternative<Tokenizer::IncludeDirective>(token.m_Value)) {
				if (!ExecuteIncludeDirective(token))
					return Tokenizer::Token{Tokenizer::SpecialPurpose::Error, token.m_Span};
				continue;
			}

			using Directive = Tokenizer::Directive;
			if (std::holds_alternative<Directive>(token.m_Value)) {
				auto const directive{std::get<Directive>(token.m_Value)};

				switch (directive) {
					case Directive::Define:
						if (!ExecuteMacroDefinition()) {
							return Tokenizer::Token{Tokenizer::SpecialPurpose::Error, token.m_Span};
						}
						continue;
					default:
						m_Diagnoses.emplace_back(token.m_Span, Diagnosis::Class::Warning, Diagnosis::Kind::TODO);
						continue;
				}
			}

			bool const isIdentifier{std::holds_alternative<Tokenizer::Identifier>(token.m_Value)};

			if (isIdentifier || std::holds_alternative<Tokenizer::Keyword>(token.m_Value)) {
				auto const identifierString{
				        isIdentifier ? std::get<Tokenizer::Identifier>(token.m_Value).m_Name
				                     : Tokenizer::Identifier::IdentString{Tokenizer::KeywordAsIdentString(
				                               std::get<Tokenizer::Keyword>(token.m_Value)
				                       )}
				};

				if (MacroRecursionCheck(token.m_Span, identifierString))
					return token;

				if (auto const &definitionIt{m_MacroDefinitions.find(identifierString)};
				    definitionIt != m_MacroDefinitions.end()) {
					if (std::holds_alternative<FunctionLikeMacro>(definitionIt->second)) {
						auto identifierToken{token};
						token = GetNextToken();
						if (!token.IsPunctuatorKind(Tokenizer::Punctuator::PpLeftParenthesis) &&
						    !token.IsPunctuatorKind(Tokenizer::Punctuator::LeftParenthesis)) {
							m_UnusedPeekResult = token;
							return identifierToken;
						}
					}

					if (!StartMacroExpansion(definitionIt->second))
						return Tokenizer::Token{Tokenizer::SpecialPurpose::Error, token.m_Span};

					continue;
				}

				if (!m_MacroStack.empty()) {
					if (!HandleMacroParameterUsage(identifierString))
						return token;

					continue;
				}

				return token;
			}

			if (std::holds_alternative<Tokenizer::Punctuator>(token.m_Value)) {
				switch (std::get<Tokenizer::Punctuator>(token.m_Value)) {
					case Tokenizer::Punctuator::PpLeftParenthesis:
						return Tokenizer::Token{Tokenizer::Punctuator::LeftParenthesis, token.m_Span};
					case Tokenizer::Punctuator::Hash: {
						if (!m_MacroStack.empty() &&
						    std::holds_alternative<FunctionLikeMacro>(m_MacroStack.top().m_Macro)) {
							return ExecuteHashOperator();
						}
					}
					default:
						break;
				}
			}

			return token;
		}
	}

	bool Preprocessor::CheckArgumentCountValidity(bool isVA, size_t paramCount, size_t argCount) noexcept {
		if (isVA) {
			return argCount >= paramCount;
		}

		return argCount == paramCount;
	}

	bool Preprocessor::GatherArgumentList(
	        bool isVA, size_t numParameters, Preprocessor::MacroReplacementStackData &replacementData
	) {
		using Punctuator = Tokenizer::Punctuator;

		Tokenizer::Token token{GetNextToken()};
		replacementData.m_Arguments = Preprocessor::MacroReplacementStackData::ArgumentList::value_type{};

		ReplacementList currentArgument{};
		int             leftParenthesesSeen{0};
		while (true) {
			if (std::holds_alternative<Tokenizer::Identifier>(token.m_Value)) {
				auto const &identifier{std::get<Tokenizer::Identifier>(token.m_Value).m_Name};

				if (auto const &vaArgs{m_MacroDefinitions.find(identifier)}; vaArgs != m_MacroDefinitions.cend()) {
					if (!StartMacroExpansion(vaArgs->second))
						return false;

					token = GetNextToken();
					continue;
				}
			} else if (std::holds_alternative<Punctuator>(token.m_Value)) {
				auto const &punctuator{std::get<Punctuator>(token.m_Value)};

				switch (punctuator) {
					case Punctuator::PpLeftParenthesis:
					case Punctuator::LeftParenthesis:
						++leftParenthesesSeen;
						break;
					case Punctuator::RightParenthesis:
						// On the last right parenthesis
						if (leftParenthesesSeen-- <= 0) {
							if (!currentArgument.m_ReplacementList.empty()) {
								replacementData.m_Arguments->emplace_back(std::move(currentArgument));
							}

							if (isVA && replacementData.m_Arguments->size() > numParameters) {
								ReplacementList::TokenList const &replacementList =
								        (replacementData.m_Arguments->cend() - 1)->m_ReplacementList;

								ObjectLikeMacro vaArgs{VA_ARGS_MACRO_NAME, {replacementList}};
								replacementData.m_VaArgsMacro = std::move(vaArgs);
							}

							if (!CheckArgumentCountValidity(isVA, numParameters, replacementData.m_Arguments->size())) {
								m_Diagnoses.emplace_back(
								        token.m_Span, Diagnosis::Class::Error,
								        Diagnosis::Kind::PP_UnexpectedMacroInvocationArgumentCount
								);
								return false;
							}


							return true;
						}

						break;
					case Punctuator::Comma:
						if (isVA && replacementData.m_Arguments->size() >= numParameters) {
							currentArgument.m_ReplacementList.emplace_back(std::move(token));
							token = GetNextToken();
							continue;
						}

						if (leftParenthesesSeen == 0) {
							replacementData.m_Arguments->emplace_back(std::move(currentArgument));
							token = GetNextToken();
							continue;
						}
						break;
					default:
						break;
				}
			}

			if (token.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::NewLine)) {
				m_Diagnoses.emplace_back(
				        token.m_Span, Diagnosis::Class::Error, Diagnosis::Kind::PP_UnterminatedMacroInvocation
				);
				return false;
			}

			currentArgument.m_ReplacementList.emplace_back(std::move(token));
			token = GetNextToken();
		}
	}

	bool Preprocessor::StartMacroExpansion(Macro const &macro) {
		if (std::holds_alternative<ObjectLikeMacro>(macro)) {
			auto const &objMacro{std::get<ObjectLikeMacro>(macro)};
			m_MacroStack.emplace(objMacro);
			m_MacroDefinitions.erase(VA_ARGS_MACRO_NAME);
			return true;
		}

		FunctionLikeMacro const &fnMacro{std::get<FunctionLikeMacro>(macro)};

		MacroReplacementStackData replacementStackData{fnMacro, {}};
		if (!GatherArgumentList(fnMacro.m_IsVA, fnMacro.m_ParameterList.size(), replacementStackData))
			return false;

		if (fnMacro.m_IsVA && replacementStackData.m_VaArgsMacro.has_value()) {
			m_MacroDefinitions.emplace(VA_ARGS_MACRO_NAME, replacementStackData.m_VaArgsMacro.value());
		}

		ExpandMacroArguments(replacementStackData.m_Arguments);

		m_MacroStack.emplace(std::move(replacementStackData));
		return true;
	}

	std::optional<int> Preprocessor::FindParameterIndexByName(
	        Preprocessor::FunctionLikeMacro::ParameterList const &parameterList,
	        Tokenizer::Identifier::IdentString const             &identifier
	) noexcept {
		for (std::size_t idx{}; idx < parameterList.size(); ++idx) {
			if (parameterList[idx].m_Name == identifier) {
				return idx;
			}
		}

		return std::nullopt;
	}

	bool Preprocessor::HandleMacroParameterUsage(Tokenizer::Identifier::IdentString const &parameterName) {
		auto &top{m_MacroStack.top()};

		if (!std::holds_alternative<FunctionLikeMacro>(top.m_Macro)) {
			return false;
		}

		auto const &fnMacro{std::get<FunctionLikeMacro>(top.m_Macro)};

		auto const argumentIndex{FindParameterIndexByName(fnMacro.m_ParameterList, parameterName)};

		if (!argumentIndex.has_value())
			return false;

		auto const &replacementList = top.m_Arguments.value()[*argumentIndex].m_ReplacementList;
		top.m_ArgumentTokenIter     = replacementList.cbegin();
		top.m_ArgumentTokenIterEnd  = replacementList.cend();

		return true;
	}

	bool
	Preprocessor::MacroRecursionCheck(Span const &currentSpan, Tokenizer::Identifier::IdentString const &identifier) {
		if (m_MacroStack.empty()) {
			return false;
		}

		auto const &macro{m_MacroStack.top().m_Macro};
		if (std::holds_alternative<ObjectLikeMacro>(macro)) {
			if (std::get<ObjectLikeMacro>(macro).m_MacroName != identifier)
				return false;
		} else {
			if (std::get<FunctionLikeMacro>(macro).m_MacroName != identifier)
				return false;
		}

		m_Diagnoses.emplace_back(
		        currentSpan, Diagnosis::Class::Warning, Diagnosis::Kind::PP_MacroDefinedInTermsOfItself
		);

		return true;
	}

	void Preprocessor::ExpandMacroArguments(MacroReplacementStackData::ArgumentList &argList) {
		if (m_MacroStack.empty() || !std::holds_alternative<FunctionLikeMacro>(m_MacroStack.top().m_Macro)) {
			return;
		}

		FunctionLikeMacro const &topFnLikeMacro{std::get<FunctionLikeMacro>(m_MacroStack.top().m_Macro)};

		for (auto &argReplacementList: argList.value()) {
			for (auto argTokenIt = argReplacementList.m_ReplacementList.begin();
			     argTokenIt != argReplacementList.m_ReplacementList.end(); ++argTokenIt) {
				auto const &argToken = *argTokenIt;

				if (!std::holds_alternative<Tokenizer::Identifier>(argToken.m_Value)) {
					continue;
				}

				auto const &argIdentifier{std::get<Tokenizer::Identifier>(argToken.m_Value)};
				auto const  idx{FindParameterIndexByName(topFnLikeMacro.m_ParameterList, argIdentifier.m_Name)};

				if (!idx.has_value())
					continue;

				ReplacementList::TokenList &replacementList =
				        m_MacroStack.top().m_Arguments.value()[*idx].m_ReplacementList;
				*argTokenIt = replacementList[0];

				std::copy(
				        replacementList.cbegin() + 1, replacementList.cend(),
				        std::inserter(argReplacementList.m_ReplacementList, argTokenIt)
				);
			}
		}
	}
}// namespace jcc
