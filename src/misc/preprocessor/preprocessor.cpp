#include "preprocessor.h"

namespace jcc::preprocessor {
	Tokenizer::Token Preprocessor::operator()() {
		while (true) {
			if (auto const token{GetNextFromTokenizer()};
			    !token.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::NewLine)) {
				return token;
			}
		}
	}

	void Preprocessor::Diagnose(Diagnosis::Class diagClass, Diagnosis::Kind kind, Span const &span) const {
		m_pDiagnoses->emplace_back(span, diagClass, kind);
	}

	Tokenizer::Token Preprocessor::GetNextFromTokenizer(bool executeCommands) {
		while (true) {
			auto token{m_Tokenizer()};

			if (std::holds_alternative<Tokenizer::SpecialPurpose>(token.m_Value)) {
				switch (std::get<Tokenizer::SpecialPurpose>(token.m_Value)) {
					case Tokenizer::SpecialPurpose::EndOfFile:
					case Tokenizer::SpecialPurpose::Error:
						return token;
					default:
						break;
				}
			}

			if (!executeCommands)
				return token;

			auto const valueType{token.GetValueType()};
			auto const commandMap{PreprocessorCommandSingleton::GetInstance().GetCommandMap()};

			if (auto const command{commandMap.find(valueType)}; command != commandMap.end()) {
				if (auto const result{command->second->Execute(*this)}; result.has_value())
					// If the command does not return a token, it means it was a directive.
					return result.value();
			} else {
				return token;
			}
		}
	}

	void Preprocessor::RegisterMacro(std::string const &name, macro::Macro macro) {
		m_MacroDefinitions.emplace(name, std::move(macro));
	}

	Preprocessor::Preprocessor(std::string const &filename, std::istream &ifstream, Diagnosis::Vec &diagnoses)
	    : m_Tokenizer{filename, ifstream, diagnoses}
	    , m_pDiagnoses{&diagnoses} {
	}
}// namespace jcc::preprocessor
