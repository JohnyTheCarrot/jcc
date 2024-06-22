#include "preprocessor.h"

namespace jcc::preprocessor {
	PreprocessorToken Preprocessor::GetNextPreprocessorToken() {
		while (true) {
			if (auto const ppToken{GetNextFromTokenizer()};
			    !ppToken.m_Token.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::NewLine)) {
				return ppToken;
			}
		}
	}

	PreprocessorIterator Preprocessor::begin() {
		return PreprocessorIterator{*this};
	}

	PreprocessorIterator Preprocessor::end() {
		return PreprocessorIterator::end();
	}

	Span const &Preprocessor::GetCurrentSpan() const noexcept {
		return m_CurrentSpan;
	}

	PreprocessorToken Preprocessor::GetNextFromTokenizer(bool executeCommands) {
		while (true) {
			auto ppToken{[this]() -> PreprocessorToken {
				if (auto const token{GetTokenFromMacroArgumentReader()}; token.has_value())
					return {token.value(), true};

				if (auto const token{GetTokenFromMacroStack()}; token.has_value())
					return {token.value(), true};

				return {m_Tokenizer(), false};
			}()};
			auto const &[token, span]{ppToken.m_Token};

			m_CurrentSpan = span;

			if (std::holds_alternative<Tokenizer::SpecialPurpose>(token)) {
				switch (std::get<Tokenizer::SpecialPurpose>(token)) {
					case Tokenizer::SpecialPurpose::EndOfFile:
					case Tokenizer::SpecialPurpose::Error:
						return ppToken;
					default:
						break;
				}
			}

			if (!executeCommands)
				return ppToken;

			auto const valueType{ppToken.m_Token.GetValueType()};
			auto const commandMap{commands::PreprocessorCommandSingleton::GetInstance().GetCommandMap()};

			if (auto const command{commandMap.find(valueType)}; command != commandMap.end()) {
				if (auto const result{command->second->Execute(*this, std::move(ppToken.m_Token))}; result.has_value())
					// If the command does not return a token, it means it was a directive.
					return result.value();
			} else {
				return ppToken;
			}
		}
	}

	void Preprocessor::RegisterMacro(std::string const &name, macro::Macro macro) {
		m_MacroDefinitions.emplace(name, std::move(macro));
	}

	void Preprocessor::InvokeMacro(macro::MacroInvocation &&macroInvocation) {
		m_MacroStack.push(macroInvocation);
	}

	void Preprocessor::UseMacroArguments(std::vector<Tokenizer::Token> &&args) {
		if (m_MacroStack.empty())
			throw std::runtime_error{"Macro argument reader is set, but the macro stack is empty"};

		auto &currentMacro{m_MacroStack.top()};
		currentMacro.m_CurrentArgReader = macro::MacroArgumentReader{std::move(args)};
	}

	int Preprocessor::GetMacroDepth() const noexcept {
		return static_cast<int>(m_MacroStack.size());
	}

	macro::Macro const *Preprocessor::GetMacro(std::string const &name, bool allowCurrentMacro) const {
		if (auto const macroIt{std::ranges::find_if(
		            m_MacroDefinitions,
		            [&](auto &pair) {
			            if (!m_MacroStack.empty()) {
				            auto const &topInvokedMacro{m_MacroStack.top()};
				            if (pair.first == topInvokedMacro.m_MacroName)
					            return allowCurrentMacro;
			            }

			            return pair.first == name;
		            }
		    )};
		    macroIt != m_MacroDefinitions.end()) {
			return &macroIt->second;
		}

		return nullptr;
	}

	std::optional<std::vector<Tokenizer::Token>> Preprocessor::GetMacroArgument(std::string const &argumentName) const {
		if (m_MacroStack.empty())
			return std::nullopt;

		auto const macroTop{m_MacroStack.top()};
		if (!macroTop.m_Args.has_value())
			return std::nullopt;

		auto const argIt{std::ranges::find_if(macroTop.m_Args.value(), [&](auto const &arg) {
			return arg.first == argumentName;
		})};

		if (argIt == macroTop.m_Args.value().end())
			return std::nullopt;

		return argIt->second;
	}

	std::optional<Tokenizer::Token> Preprocessor::GetTokenFromMacroStack() {
		if (m_MacroStack.empty())
			return std::nullopt;

		auto &[m_MacroName, m_CurrentTokenIndex, m_Args, m_CurrentArgReader]{m_MacroStack.top()};
		++m_CurrentTokenIndex;

		if (auto const *macro{GetMacro(m_MacroName, true)}; macro != nullptr) {
			auto const [m_ReplacementList]{[macro] {
				if (std::holds_alternative<macro::ObjectLikeMacro>(*macro)) {
					return std::get<macro::ObjectLikeMacro>(*macro).m_ReplacementList;
				}

				return std::get<macro::FunctionLikeMacro>(*macro).m_ReplacementList;
			}()};

			if (m_CurrentTokenIndex >= static_cast<int>(m_ReplacementList.size())) {
				m_MacroStack.pop();
				return GetTokenFromMacroStack();
			}

			return m_ReplacementList.at(m_CurrentTokenIndex);
		}

		throw std::runtime_error{"Macro stack is not empty, but the macro is not found"};
	}

	std::optional<Tokenizer::Token> Preprocessor::GetTokenFromMacroArgumentReader() {
		if (m_MacroStack.empty())
			return std::nullopt;

		auto &macroArgReader{m_MacroStack.top().m_CurrentArgReader};

		if (!macroArgReader.has_value())
			return std::nullopt;

		auto &[m_Args, m_CurrentTokenIndex]{macroArgReader.value()};

		++m_CurrentTokenIndex;

		if (m_CurrentTokenIndex >= static_cast<int>(m_Args.size())) {
			macroArgReader = std::nullopt;
			return std::nullopt;
		}

		return m_Args.at(m_CurrentTokenIndex);
	}

	Preprocessor::Preprocessor(std::string const &filename, std::istream &ifstream, Diagnosis::Vec &diagnoses)
	    : m_Tokenizer{filename, ifstream, diagnoses}
	    , m_pDiagnoses{&diagnoses}
	    , m_CurrentSpan{std::make_shared<std::string>(filename), {}, {}, {}, &ifstream} {
	}
}// namespace jcc::preprocessor
