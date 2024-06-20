#ifndef PREPROCESSOR_ITERATOR_H
#define PREPROCESSOR_ITERATOR_H

#include "../Tokenizer.h"
#include <iterator>
#include <variant>

namespace jcc::preprocessor {
	class Preprocessor;

	template<class Super>
	class BasePreprocessorIterator {
		using UntilCondition = std::function<bool(Tokenizer::Token const &)>;

		mutable std::variant<Tokenizer::Token::Type, Tokenizer::Token, UntilCondition> m_Token;

	protected:
		mutable Preprocessor *m_pPreprocessor{};

		explicit BasePreprocessorIterator(Tokenizer::Token::Type untilType)
		    : m_Token{untilType} {
		}

		explicit BasePreprocessorIterator(UntilCondition const &untilCondition)
		    : m_Token{untilCondition} {
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

		static Super Until(UntilCondition const &untilCondition) {
			return Super{untilCondition};
		}

		[[nodiscard]]
		bool
		operator==(Super const &other) const noexcept {
			if (std::holds_alternative<UntilCondition>(other.m_Token)) {
				if (!std::holds_alternative<Tokenizer::Token>(m_Token))
					return true;// both are special purpose

				auto const condition{std::get<UntilCondition>(other.m_Token)};

				return condition(std::get<Tokenizer::Token>(m_Token));
			}

			if (std::holds_alternative<Tokenizer::Token::Type>(other.m_Token)) {
				if (!std::holds_alternative<Tokenizer::Token>(m_Token))
					return true;// both are special purpose

				auto const currentTokenType{std::get<Tokenizer::Token>(m_Token).GetValueType()};
				auto const isCurrentEOF{
				        std::holds_alternative<Tokenizer::SpecialPurpose>(currentTokenType) &&
				        std::get<Tokenizer::SpecialPurpose>(currentTokenType) == Tokenizer::SpecialPurpose::EndOfFile
				};

				return isCurrentEOF || currentTokenType == std::get<Tokenizer::Token::Type>(other.m_Token);
			}

			return std::get<Tokenizer::Token>(m_Token) == std::get<Tokenizer::Token>(other.m_Token);
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
		{
			T::Until([](Tokenizer::Token const &) { return false; })
		} -> std::same_as<T>;
		{ t.GetNextToken() } -> std::same_as<Tokenizer::Token>;
	};

	class PreprocessorIterator final : public BasePreprocessorIterator<PreprocessorIterator> {
	public:
		using BasePreprocessorIterator::BasePreprocessorIterator;

		[[nodiscard]]
		Tokenizer::Token GetNextToken() const;
	};

	static_assert(std::input_iterator<PreprocessorIterator>);

	class InternalPreprocessorIterator final : public BasePreprocessorIterator<InternalPreprocessorIterator> {
	public:
		using BasePreprocessorIterator::BasePreprocessorIterator;

		[[nodiscard]]
		Tokenizer::Token GetNextToken() const;
	};

	static_assert(std::input_iterator<InternalPreprocessorIterator>);
}// namespace jcc::preprocessor

#endif//PREPROCESSOR_ITERATOR_H
