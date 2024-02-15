#include "Preprocessor.h"

void Preprocessor::ExecuteIncludeDirective(const Tokenizer::IncludeDirective &includeDirective) {
	using HeaderType = Tokenizer::IncludeDirective::HeaderType;

	std::string fileName{};

	switch (includeDirective.m_HeaderType) {
		case HeaderType::HChar:
		case HeaderType::MacroName:
			assert(false && "TODO");
			return;
		case HeaderType::QChar:
			fileName = std::get<std::string>(includeDirective.m_Name);
			break;
	}

	const auto ifStream{m_IStreams.emplace_back(std::make_unique<std::ifstream>(fileName)).get()};

	if (!*ifStream) {
		// todo: don't exit, use SpecialPurpose::Error
		std::cerr << "Couldn't open include file " << fileName << ": " << strerror(errno) << std::endl;
		exit(1);
	}

	m_CurrentTokenizer =
	        m_TokenizerStack.emplace(std::make_unique<Tokenizer>(std::move(fileName), *ifStream, m_Diagnoses)).get();
}

Tokenizer::Token Preprocessor::operator()() {
	while (true) {
		const auto token{(*m_CurrentTokenizer)()};

		if (std::holds_alternative<Tokenizer::SpecialPurpose>(token.m_Value)) {
			const auto specialPurpose{std::get<Tokenizer::SpecialPurpose>(token.m_Value)};

			if (specialPurpose == Tokenizer::SpecialPurpose::EndOfFile) {
				if (m_TokenizerStack.empty() || m_CurrentTokenizer == &m_MainTokenizer)
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
			ExecuteIncludeDirective(std::get<Tokenizer::IncludeDirective>(token.m_Value));
			continue;
		}

		return token;
	}
}
