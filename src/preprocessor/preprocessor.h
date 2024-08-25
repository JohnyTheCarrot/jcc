#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "commands/command.h"
#include "macro.h"
#include "misc/Diagnosis.h"
#include "preprocessor_iterator.h"
#include "preprocessor_token.h"
#include "tokenizer/tokenizer.h"
#include "tokenizer/tokenizer_iterator.h"

#include <optional>
#include <stack>

namespace jcc::preprocessor {
	constexpr std::string_view VaArgs{"__VA_ARGS__"};

	class Preprocessor final {
		using MacroDefinitions = std::unordered_map<std::string, macro::Macro>;

		MacroDefinitions m_MacroDefinitions{};

		tokenizer::Tokenizer               m_Tokenizer;
		tokenizer::TokenizerIterator       m_TokenIter;
		Diagnosis::Vec                    *m_pDiagnoses;
		std::stack<macro::MacroInvocation> m_MacroStack{};
		Span                               m_CurrentSpan;

		[[nodiscard]]
		std::optional<tokenizer::Token> GetTokenFromMacroStack();

		[[nodiscard]]
		std::optional<tokenizer::Token> GetTokenFromMacroArgumentReader();

	public:
		Preprocessor(std::string const &filename, std::istream &ifstream, Diagnosis::Vec &diagnoses);

		[[nodiscard]]
		PreprocessorToken GetNextFromTokenizer(bool executeCommands = true);

		void RegisterMacro(std::string const &name, macro::Macro macro);

		void InvokeMacro(macro::MacroInvocation &&macroInvocation);

		void UseMacroArguments(std::vector<tokenizer::Token> &&args);

		[[nodiscard]]
		int GetMacroDepth() const noexcept;

		[[nodiscard]]
		macro::Macro const *GetMacro(std::string const &name, bool allowCurrentMacro = false) const;

		[[nodiscard]]
		std::optional<std::vector<tokenizer::Token>> GetMacroArgument(std::string const &argumentName) const;

		[[nodiscard]]
		PreprocessorToken GetNextPreprocessorToken();

		template<class It = PreprocessorIterator>
		    requires IsPreprocessorIterator<It> or std::same_as<It, InternalPreprocessorIterator>
		[[nodiscard]]
		It Current() {
			return It{*this};
		}

		template<class It = PreprocessorIterator>
		    requires IsPreprocessorIterator<It> or std::same_as<It, InternalPreprocessorIterator>
		[[nodiscard]]
		It EndOfFile() {
			return It::end();
		}

		template<class It = PreprocessorIterator>
		    requires IsPreprocessorIterator<It> or std::same_as<It, InternalPreprocessorIterator>
		[[nodiscard]]
		It Until(tokenizer::Token::Type untilType) {
			return It::Until(untilType);
		}

		template<class It = PreprocessorIterator>
		    requires IsPreprocessorIterator<It> or std::same_as<It, InternalPreprocessorIterator>
		[[nodiscard]]
		It Until(std::function<bool(tokenizer::Token const &)> const &untilCondition) {
			return It::Until(untilCondition);
		}

		PreprocessorIterator begin();

		PreprocessorIterator end();

		[[nodiscard]]
		Span const &GetCurrentSpan() const noexcept;
	};
}// namespace jcc::preprocessor

#endif// PREPROCESSOR_H
