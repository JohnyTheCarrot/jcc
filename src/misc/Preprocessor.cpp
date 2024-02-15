#include "Preprocessor.h"

bool Preprocessor::ExecuteIncludeDirective(const Tokenizer::Token &includeDirectiveToken) {
	using HeaderType = Tokenizer::IncludeDirective::HeaderType;

	std::string fileName{};

	const auto directive{std::get<Tokenizer::IncludeDirective>(includeDirectiveToken.m_Value)};

	switch (directive.m_HeaderType) {
		case HeaderType::HChar:
		case HeaderType::MacroName:
			m_Diagnoses.emplace_back(includeDirectiveToken.m_Span, Diagnosis::Class::Warning, Diagnosis::Kind::TODO);
			return true;
		case HeaderType::QChar:
			fileName = std::get<std::string>(directive.m_Name);
			break;
	}

	const auto ifStream{m_IStreams.emplace_back(std::make_unique<std::ifstream>(fileName)).get()};

	if (!*ifStream) {
		m_Diagnoses.emplace_back(
		        includeDirectiveToken.m_Span, Diagnosis::Class::Error, Diagnosis::Kind::PP_InclDirectiveFileOpenFailed
		);
		return false;
	}

	m_CurrentTokenizer =
	        m_TokenizerStack.emplace(std::make_unique<Tokenizer>(std::move(fileName), *ifStream, m_Diagnoses)).get();
	return true;
}

Tokenizer::Token Preprocessor::operator()() {
	while (true) {
		auto token{(*m_CurrentTokenizer)()};

		if (std::holds_alternative<Tokenizer::SpecialPurpose>(token.m_Value)) {
			const auto specialPurpose{std::get<Tokenizer::SpecialPurpose>(token.m_Value)};

			if (specialPurpose == Tokenizer::SpecialPurpose::EndOfFile) {
				if (m_TokenizerStack.empty())
					return token;// eof

				m_TokenizerStack.pop();
				if (m_TokenizerStack.empty())
					m_CurrentTokenizer = &m_MainTokenizer;
				else
					m_CurrentTokenizer = m_TokenizerStack.top().get();
				continue;
			}

			return token;
		}

		if (std::holds_alternative<Tokenizer::IncludeDirective>(token.m_Value)) {
			if (!ExecuteIncludeDirective(token))
				return Tokenizer::Token{Tokenizer::SpecialPurpose::Error, token.m_Span};
			continue;
		}

		return token;
	}
}
