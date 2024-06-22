#include "preprocessor_iterator.h"
#include "preprocessor.h"

namespace jcc::preprocessor {
	Tokenizer::Token PreprocessorIterator::GetNextToken() const {
		return m_pPreprocessor->GetNextPreprocessorToken().m_Token;
	}

	Tokenizer::Token PreprocessorIteratorNoCommands::GetNextToken() const {
		return m_pPreprocessor->GetNextFromTokenizer(false).m_Token;
	}

	InternalPreprocessorIterator::InternalPreprocessorIterator(Preprocessor &preprocessor)
	    : m_pPreprocessor{&preprocessor} {
		++(*this);
	}

	InternalPreprocessorIterator::reference InternalPreprocessorIterator::operator*() const {
		return std::get<PreprocessorToken>(m_Token);
	}

	InternalPreprocessorIterator::pointer InternalPreprocessorIterator::operator->() const {
		return &std::get<PreprocessorToken>(m_Token);
	}

	InternalPreprocessorIterator &InternalPreprocessorIterator::operator++() {
		auto const ppToken{m_pPreprocessor->GetNextFromTokenizer(true)};

		if (auto const &[token, isFromMacro]{ppToken};
		    token.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::EndOfFile) ||
		    (std::holds_alternative<Tokenizer::Token::Type>(m_Token) &&
		     std::get<Tokenizer::Token::Type>(m_Token) == token.GetValueType()))
			m_Token = token.GetValueType();
		else
			m_Token = ppToken;

		return *this;
	}

	InternalPreprocessorIterator InternalPreprocessorIterator::operator++(int) {
		InternalPreprocessorIterator tmp{*this};
		++(*this);
		return tmp;
	}

	InternalPreprocessorIterator InternalPreprocessorIterator::begin() const {
		return InternalPreprocessorIterator{*m_pPreprocessor};
	}

	InternalPreprocessorIterator InternalPreprocessorIterator::end() {
		return InternalPreprocessorIterator{};
	}

	bool InternalPreprocessorIterator::operator==(InternalPreprocessorIterator const &other) const noexcept {
		return m_Token == other.m_Token;
	}

	bool InternalPreprocessorIterator::operator!=(InternalPreprocessorIterator const &other) const noexcept {
		return !(*this == other);
	}

	InternalPreprocessorIterator::InternalPreprocessorIterator() = default;
}// namespace jcc::preprocessor
