#include "tokenizer/tokenizer.h"
#include "misc/Diagnosis.h"
#include "tokens/identifiers.h"
#include "tokens/static_tokens.h"

namespace jcc::tokenizer {
	void Tokenizer::ExpectNoEof(Span &span) const {
		if (m_CharIter == CharIter::end()) {
			span.m_End = m_CharIter.GetSentinel().m_LastSpanMarker;
			throw FatalCompilerError{Diagnosis::Kind::UnexpectedEOF, std::move(span)};
		}
	}

	bool Tokenizer::SkipWhitespace() {
		CharIter const untilNewline{'\n'};
		bool           skippedWhitespace{false};

		while (m_CharIter != untilNewline && std::isspace(m_CharIter->m_Char)) {
			++m_CharIter;
			skippedWhitespace = true;
		}

		return skippedWhitespace;
	}

	Tokenizer::Tokenizer(std::istream &input, std::string_view fileName)
	    : m_CharIter{input, fileName} {
	}

	std::optional<Token> Tokenizer::GetNextToken() {
		if (m_CharIter == CharIter::end())
			return std::nullopt;

		bool const skippedWhitespace{SkipWhitespace()};

		if (m_CharIter == CharIter::end())
			return std::nullopt;

		Span span{
		        m_CharIter.GetFileName(), m_CharIter->m_SpanMarker, m_CharIter->m_SpanMarker,
		        m_CharIter.GetInput()->tellg(), m_CharIter.GetInput()
		};
		if (m_CharIter->m_Char == '\n') {
			return Token{SpecialPurpose::NewLine, std::move(span)};
		}

		bool const couldBeIdentifier{Identifier::IsValidFirstChar(m_CharIter->m_Char)};
		auto [valueOrString, trieResultEndMarker]{static_tokens::Tokenize(m_CharIter)};
		span.m_End = trieResultEndMarker;

		if (couldBeIdentifier && std::holds_alternative<std::string>(valueOrString)) {
			auto &partialIdentifier{std::get<std::string>(valueOrString)};

			return identifiers::Tokenize(
			        m_CharIter,
			        identifiers::IdentifierTokenStart{
			                .m_Identifier = std::move(partialIdentifier),
			                .m_Start      = span.m_Start,
			                .m_StartPos   = span.m_StartPos,
			        }
			);
		}

		auto const tokenValue{[&]() -> Token::Value {
			if (!std::holds_alternative<Token::Value>(valueOrString))
				throw FatalCompilerError{Diagnosis::Kind::TK_UnexpectedChar, std::move(span)};

			auto tokenValue{std::get<Token::Value>(valueOrString)};
			if (std::holds_alternative<Punctuator>(tokenValue)) {
				if (auto const punctuator{std::get<Punctuator>(tokenValue)};
				    punctuator == Punctuator::PpLeftParenthesis && skippedWhitespace)
					tokenValue = Punctuator::LeftParenthesis;
			}

			return tokenValue;
		}()};

		return Token{.m_Value = tokenValue, .m_Span = std::move(span)};
	}

	TokenizerIterator Tokenizer::begin() {
		return TokenizerIterator{*this};
	}

	TokenizerIterator Tokenizer::end() {
		return TokenizerIterator{};
	}
}// namespace jcc::tokenizer
