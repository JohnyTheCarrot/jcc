#ifndef PREPROCESSOR_ITERATOR_H
#define PREPROCESSOR_ITERATOR_H

#include "../Tokenizer.h"
#include <iterator>
#include <variant>

namespace jcc::preprocessor {
	class Preprocessor;

	template<class Super>
	class BasePreprocessorIterator {
		struct Sentinel final {};

		mutable std::variant<Tokenizer::Token::Type, Tokenizer::Token> m_Token{};

	protected:
		mutable Preprocessor *m_pPreprocessor{};

		explicit BasePreprocessorIterator(Tokenizer::Token::Type untilType)
		    : m_Token{untilType} {
		}

	public:
		using iterator_category = std::input_iterator_tag;
		using value_type        = Tokenizer::Token;
		using reference         = value_type &;
		using pointer           = value_type *;
		using difference_type   = int;

		explicit BasePreprocessorIterator(Preprocessor &preprocessor)
		    : m_Token{Tokenizer::SpecialPurpose::EndOfFile}
		    , m_pPreprocessor{&preprocessor} {
			++(*this);
		}

		BasePreprocessorIterator()
		    : m_Token{Tokenizer::SpecialPurpose::EndOfFile} {
		}

		reference operator*() const {
			return std::get<Tokenizer::Token>(m_Token);
		}

		pointer operator->() const {
			return &std::get<Tokenizer::Token>(m_Token);
		}

		Super &operator++() {
			auto const token{static_cast<Super *>(this)->GetNextToken()};

			if (token.IsSpecialPurposeKind(Tokenizer::SpecialPurpose::EndOfFile) ||
			    (std::holds_alternative<Tokenizer::Token::Type>(m_Token) &&
			     std::get<Tokenizer::Token::Type>(m_Token) == token.GetValueType()))
				m_Token = token.GetValueType();
			else
				m_Token = token;

			return static_cast<Super &>(*this);
		}

		Super operator++(int) {
			Super temp{*static_cast<Super *>(this)};
			++(*this);
			return temp;
		}

		Super begin() const {
			return Super{*m_pPreprocessor};
		}

		static Super end() {
			return Super{};
		}

		static Super Until(Tokenizer::Token::Type untilType) {
			return Super{untilType};
		}

		[[nodiscard]]
		bool
		operator==(Super const &other) const noexcept {
			if (std::holds_alternative<Tokenizer::Token::Type>(m_Token)) {
				auto const currentTokenType{std::get<Tokenizer::Token::Type>(m_Token)};
				auto const isCurrentEOF{
				        std::holds_alternative<Tokenizer::SpecialPurpose>(currentTokenType) &&
				        std::get<Tokenizer::SpecialPurpose>(currentTokenType) == Tokenizer::SpecialPurpose::EndOfFile
				};

				if (std::holds_alternative<Tokenizer::Token::Type>(other.m_Token)) {
					auto const otherTokenType{std::get<Tokenizer::Token::Type>(other.m_Token)};

					return isCurrentEOF || currentTokenType == otherTokenType;
				}

				return std::get<Tokenizer::Token::Type>(m_Token) ==
				       std::get<Tokenizer::Token>(other.m_Token).GetValueType();
			}

			if (std::holds_alternative<Tokenizer::Token::Type>(other.m_Token)) {
				auto const currentTokenType{std::get<Tokenizer::Token>(m_Token).GetValueType()};

				return currentTokenType == std::get<Tokenizer::Token::Type>(other.m_Token);
			}

			return m_Token == other.m_Token;
		}

		[[nodiscard]]
		bool
		operator!=(Super const &other) const noexcept {
			return !(*this == other);
		}
	};

	template<class T>
	concept IsPreprocessorIterator = requires(T t) {
		requires std::input_iterator<T> and std::default_initializable<T>;
		{ T::end() } -> std::same_as<T>;
		{ T::Until(Tokenizer::SpecialPurpose::EndOfFile) } -> std::same_as<T>;
		{ t.GetNextToken() } -> std::same_as<Tokenizer::Token>;
	};

	class PreprocessorIterator final : public BasePreprocessorIterator<PreprocessorIterator> {
	public:
		using BasePreprocessorIterator::BasePreprocessorIterator;

		[[nodiscard]]
		Tokenizer::Token GetNextToken() const;

		explicit PreprocessorIterator(Preprocessor &preprocessor)
		    : BasePreprocessorIterator{preprocessor} {
		}
	};

	class InternalPreprocessorIterator final : public BasePreprocessorIterator<InternalPreprocessorIterator> {
	public:
		using BasePreprocessorIterator::BasePreprocessorIterator;

		[[nodiscard]]
		Tokenizer::Token GetNextToken() const;
	};
}// namespace jcc::preprocessor

#endif//PREPROCESSOR_ITERATOR_H
