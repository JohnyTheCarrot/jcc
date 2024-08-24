#include "tokenizer/tokenizer.h"
#include "misc/Diagnosis.h"
#include "tokens/character_constants.h"
#include "tokens/identifiers.h"
#include "tokens/static_tokens.h"
#include "tokens/string_literals.h"

namespace jcc::tokenizer {
	void Tokenizer::ExpectNoEof(Span &span) const {
		if (m_CharIter == CharIter::end()) {
			span.m_End = m_CharIter.GetSentinel().m_LastSpanMarker;
			throw FatalCompilerError{Diagnosis::Kind::UnexpectedEOF, std::move(span)};
		}
	}

	bool Tokenizer::SkipWhitespace() {
		auto const newIt{std::find_if_not(m_CharIter, CharIter::c_UntilNewline, [](CharInfo const &charInfo) {
			return std::isspace(charInfo.m_Char);
		})};

		bool const didSkipWhitespace{m_CharIter != newIt};
		m_CharIter = newIt;

		return didSkipWhitespace;
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
		        m_CharIter.GetFileName(), m_CharIter.GetCurrentSpanMarker(), m_CharIter.GetCurrentSpanMarker(),
		        m_CharIter.GetInput()->tellg(), m_CharIter.GetInput()
		};
		if (m_CharIter->m_Char == '\n') {
			return Token{SpecialPurpose::NewLine, std::move(span)};
		}

		if (m_CharIter->m_Char == '\'') {
			++m_CharIter;
			return character_constants::Tokenize(m_CharIter, ConstantPrefix::None, span.m_Start);
		}

		if (m_CharIter->m_Char == '"') {
			++m_CharIter;
			return string_literals::Tokenize(m_CharIter, ConstantPrefix::None, span.m_Start);
		}

		bool const couldBeIdentifier{Identifier::IsValidFirstChar(m_CharIter->m_Char)};
		auto [valueOrString, trieResultEndMarker]{static_tokens::Tokenize(m_CharIter)};
		span.m_End = trieResultEndMarker;

		if (std::holds_alternative<std::string>(valueOrString)) {
			auto identifier{std::get<std::string>(valueOrString)};

			if (m_CharIter != CharIter::end()) {
				if (m_CharIter->m_Char == '\'') {
					++m_CharIter;
					ConstantPrefix const prefix{ToConstantPrefix(identifier)};
					return character_constants::Tokenize(m_CharIter, prefix, span.m_Start);
				}

				if (m_CharIter->m_Char == '"') {
					++m_CharIter;
					ConstantPrefix const prefix{ToConstantPrefix(identifier)};
					return string_literals::Tokenize(m_CharIter, prefix, span.m_Start);
				}
			}

			span.m_End = identifiers::CollectRestOfIdentifier(m_CharIter, identifier).value_or(span.m_End);

			if (couldBeIdentifier)
				return identifiers::Tokenize(
				        m_CharIter,
				        identifiers::IdentifierTokenStart{
				                .m_Identifier = std::move(identifier),
				                .m_Start      = span.m_Start,
				                .m_StartPos   = span.m_StartPos,
				        }
				);
		}

		auto const tokenValue{[&]() -> Token::Value {
			if (!std::holds_alternative<Token::Value>(valueOrString))
				throw FatalCompilerError{Diagnosis::Kind::TK_UnexpectedChar, std::move(span)};

			auto value{std::get<Token::Value>(valueOrString)};
			if (std::holds_alternative<Punctuator>(value)) {
				if (auto const punctuator{std::get<Punctuator>(value)};
				    punctuator == Punctuator::PpLeftParenthesis && skippedWhitespace)
					value = Punctuator::LeftParenthesis;
			}

			return value;
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
