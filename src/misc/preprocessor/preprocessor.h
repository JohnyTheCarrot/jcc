#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "../Tokenizer.h"
#include "commands/command.h"
#include "macro.h"
#include "preprocessor_iterator.h"
#include <optional>

namespace jcc::preprocessor {
	constexpr std::string_view VaArgs{"__VA_ARGS__"};

	class Preprocessor final {
		using MacroDefinitions = std::unordered_map<std::string, macro::Macro>;

		MacroDefinitions m_MacroDefinitions{};

		Tokenizer                          m_Tokenizer;
		Diagnosis::Vec                    *m_pDiagnoses;
		std::stack<macro::MacroInvocation> m_MacroStack{};
		Span                               m_CurrentSpan;

		[[nodiscard]]
		std::optional<Tokenizer::Token> GetTokenFromMacroStack();

		[[nodiscard]]
		std::optional<Tokenizer::Token> GetTokenFromMacroArgumentReader();

	public:
		Preprocessor(std::string const &filename, std::istream &ifstream, Diagnosis::Vec &diagnoses);

		[[nodiscard]]
		Tokenizer::Token GetNextFromTokenizer(bool executeCommands = true);

		void RegisterMacro(std::string const &name, macro::Macro macro);

		void InvokeMacro(macro::MacroInvocation &&macroInvocation);

		void UseMacroArguments(std::vector<Tokenizer::Token> &&args);

		[[nodiscard]]
		int GetMacroDepth() const noexcept;

		[[nodiscard]]
		macro::Macro const *GetMacro(std::string const &name, bool allowCurrentMacro = false) const;

		[[nodiscard]]
		std::optional<std::vector<Tokenizer::Token>> GetMacroArgument(std::string const &argumentName) const;

		[[nodiscard]]
		Tokenizer::Token
		operator()();

		template<class It = PreprocessorIterator>
		    requires IsPreprocessorIterator<It>
		[[nodiscard]]
		It Current() {
			return It{*this};
		}

		template<class It = PreprocessorIterator>
		    requires IsPreprocessorIterator<It>
		[[nodiscard]]
		It EndOfFile() {
			return It::end();
		}

		template<class It = PreprocessorIterator>
		    requires IsPreprocessorIterator<It>
		[[nodiscard]]
		It Until(Tokenizer::Token::Type untilType) {
			return It::Until(untilType);
		}

		template<class It = PreprocessorIterator>
		    requires IsPreprocessorIterator<It>
		[[nodiscard]]
		It Until(std::function<bool(Tokenizer::Token const &)> const &untilCondition) {
			return It::Until(untilCondition);
		}

		PreprocessorIterator begin();

		PreprocessorIterator end();

		[[nodiscard]]
		Span const &GetCurrentSpan() const noexcept;
	};
}// namespace jcc::preprocessor

#endif// PREPROCESSOR_H
