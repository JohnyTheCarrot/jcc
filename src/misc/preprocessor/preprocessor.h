#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "../Tokenizer.h"
#include "command.h"
#include "macro.h"
#include "preprocessor_iterator.h"

namespace jcc::preprocessor {
	class Preprocessor final {
		using MacroDefinitions = std::unordered_map<std::string, macro::Macro>;

		MacroDefinitions m_MacroDefinitions{};
		Tokenizer        m_Tokenizer;
		Diagnosis::Vec  *m_pDiagnoses;

	public:
		Preprocessor(std::string const &filename, std::istream &ifstream, Diagnosis::Vec &diagnoses);

		[[nodiscard]]
		Tokenizer::Token GetNextFromTokenizer(bool executeCommands = true);

		void RegisterMacro(std::string const &name, macro::Macro macro);

		[[nodiscard]]
		Tokenizer::Token
		operator()();

		void Diagnose(Diagnosis::Class diagClass, Diagnosis::Kind kind, Span const &span) const;

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
	};
}// namespace jcc::preprocessor

#endif// PREPROCESSOR_H
