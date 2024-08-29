#include "tokenizer/tokenizer.h"
#include "misc/Diagnosis.h"
#include "tokens/character_constants.h"
#include "tokens/identifiers.h"
#include "tokens/pp_numbers.h"
#include "tokens/static_tokens.h"
#include "tokens/string_literals.h"

namespace jcc::tokenizer {
	bool Tokenizer::SkipWhitespace() {
		auto const newIt{std::find_if_not(m_CharIter, CharIter::c_UntilNewline, [](CharInfo const &charInfo) {
			return std::isspace(charInfo.m_Char);
		})};

		bool const didSkipWhitespace{m_CharIter != newIt};
		m_CharIter = newIt;

		return didSkipWhitespace;
	}

	void Tokenizer::SkipLineComment() {
		while (m_CharIter != CharIter::c_UntilNewline) ++m_CharIter;
	}

	void Tokenizer::SkipBlockComment(Span &span) {
		while (true) {
			if (m_CharIter == CharIter::end()) {
				span.m_End = m_CharIter.GetSentinel().m_LastSpanMarker;
				throw FatalCompilerError{Diagnosis::Kind::UnexpectedEOF, std::move(span)};
			}
			if (m_CharIter->m_Char == '*') {
				++m_CharIter;
				if (m_CharIter == CharIter::end()) {
					span.m_End = m_CharIter.GetSentinel().m_LastSpanMarker;
					throw FatalCompilerError{Diagnosis::Kind::UnexpectedEOF, std::move(span)};
				}

				if (m_CharIter->m_Char == '/') {
					++m_CharIter;
					return;
				}
			}

			++m_CharIter;
		}
	}

	std::optional<Token> Tokenizer::TokenizeStaticToken(
	        static_tokens::StaticTokenTokenizationResult::ValueOrString const &valueOrString, Span &span,
	        bool skippedWhitespace
	) {
		auto const tokenValue{[&]() -> Token::Value {
			if (!std::holds_alternative<Token::Value>(valueOrString))
				throw FatalCompilerError{Diagnosis::Kind::TK_UnexpectedChar, std::move(span), m_CharIter->m_Char};

			auto value{std::get<Token::Value>(valueOrString)};
			if (std::holds_alternative<Punctuator>(value)) {
				if (auto const punctuator{std::get<Punctuator>(value)};
				    punctuator == Punctuator::PpLeftParenthesis && skippedWhitespace)
					value = Punctuator::LeftParenthesis;
			}

			return value;
		}()};

		if (std::holds_alternative<Punctuator>(tokenValue)) {
			if (auto const punctuator{std::get<Punctuator>(tokenValue)};
			    punctuator == Punctuator::Dot && m_CharIter != CharIter::end() && std::isdigit(m_CharIter->m_Char)) {
				return pp_numbers::Tokenize(m_CharIter, span.m_Start, true);
			}
		}

		if (std::holds_alternative<SpecialPurpose>(tokenValue)) {
			auto const specialPurpose{std::get<SpecialPurpose>(tokenValue)};

			if (specialPurpose == SpecialPurpose::LineComment) {
				SkipLineComment();
				return GetNextToken();
			}

			if (specialPurpose == SpecialPurpose::BlockComment) {
				SkipBlockComment(span);
				return GetNextToken();
			}
		}

		return Token{.m_Value = tokenValue, .m_Span = std::move(span)};
	}

	Tokenizer::Tokenizer(std::istream &input, std::string_view fileName)
	    : m_CharIter{input, fileName} {
	}

	// TODO: this function is too long, refactor it
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

		switch (m_CharIter->m_Char) {
			case '\n':
				++m_CharIter;
				return Token{SpecialPurpose::NewLine, std::move(span)};
			case '\'':
				++m_CharIter;
				return character_constants::Tokenize(m_CharIter, ConstantPrefix::None, span.m_Start);
			case '"':
				++m_CharIter;
				return string_literals::Tokenize(m_CharIter, ConstantPrefix::None, span.m_Start);
			default:
				break;
		}

		if (m_CharIter != CharIter::end() && std::isdigit(m_CharIter->m_Char)) {
			return pp_numbers::Tokenize(m_CharIter, span.m_Start, false);
		}

		bool const couldBeIdentifier{Identifier::IsValidFirstChar(m_CharIter->m_Char)};
		auto [valueOrString, trieResultEndMarker]{static_tokens::Tokenize(m_CharIter)};
		span.m_End = trieResultEndMarker;

		if (std::holds_alternative<std::string>(valueOrString)) {
			auto identifier{std::get<std::string>(valueOrString)};

			span.m_End = identifiers::CollectRestOfIdentifier(m_CharIter, identifier).value_or(span.m_End);

			if (m_CharIter != CharIter::end()) {
				auto const nextChar{m_CharIter->m_Char};

				if (nextChar == '\'') {
					++m_CharIter;
					ConstantPrefix const prefix{ToConstantPrefix(identifier)};
					return character_constants::Tokenize(m_CharIter, prefix, span.m_Start);
				}

				if (nextChar == '"') {
					++m_CharIter;
					ConstantPrefix const prefix{ToConstantPrefix(identifier)};
					return string_literals::Tokenize(m_CharIter, prefix, span.m_Start);
				}
			}

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

		return TokenizeStaticToken(valueOrString, span, skippedWhitespace);
	}

	TokenizerIterator Tokenizer::begin() {
		return TokenizerIterator{*this};
	}

	TokenizerIterator Tokenizer::end() {
		return TokenizerIterator{};
	}

	Span Tokenizer::GetLastSpan() const {
		return {m_CharIter.GetFileName(), m_CharIter.GetCurrentSpanMarker(), m_CharIter.GetCurrentSpanMarker(),
		        m_CharIter.GetInput()->tellg(), m_CharIter.GetInput()};
	}
}// namespace jcc::tokenizer
