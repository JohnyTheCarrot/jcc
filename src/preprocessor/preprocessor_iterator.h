#ifndef PREPROCESSOR_ITERATOR_H
#define PREPROCESSOR_ITERATOR_H

#include <concepts>  // for same_as, default_initializable
#include <functional>// for function
#include <iterator>  // for input_iterator, input_iterator_tag
#include <variant>   // for get, holds_alternative, variant

#include "preprocessor_token.h"// for PreprocessorToken
#include "tokenizer/token.h"   // for Token, SpecialPurpose

namespace jcc::preprocessor {
    class PreprocessorIteratorVerbatim;
    class Preprocessor;

    namespace internal {
        [[nodiscard]]
        inline tokenizer::Token GetToken(PreprocessorToken const &token) {
            return pp_token::GetToken(token);
        }

        inline tokenizer::Token GetToken(tokenizer::Token const &token) {
            return token;
        }
    }// namespace internal

    template<class Super, class ValueType = tokenizer::Token>
    class BasePreprocessorIterator {
        using UntilCondition = std::function<bool(ValueType const &)>;

        mutable std::variant<tokenizer::Token::Type, ValueType, UntilCondition>
                m_Token{tokenizer::SpecialPurpose::EndOfFile};

    protected:
        mutable Preprocessor *m_pPreprocessor{};

        explicit BasePreprocessorIterator(tokenizer::Token::Type untilType)
            : m_Token{untilType} {
        }

        explicit BasePreprocessorIterator(UntilCondition const &untilCondition)
            : m_Token{untilCondition} {
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = ValueType;
        using reference         = value_type &;
        using pointer           = value_type *;
        using difference_type   = int;

        explicit BasePreprocessorIterator(Preprocessor &preprocessor)
            : m_pPreprocessor{&preprocessor} {
        }

        BasePreprocessorIterator() = default;

        reference operator*() const {
            return std::get<ValueType>(m_Token);
        }

        pointer operator->() const {
            return &std::get<ValueType>(m_Token);
        }

        Super &operator++() {
            auto const token{static_cast<Super *>(this)->GetNextToken()};

            tokenizer::Token finalToken = [&token] {
                if constexpr (std::same_as<
                                      Super, PreprocessorIteratorVerbatim>) {
                    return pp_token::GetToken(token);
                } else {
                    return token;
                }
            }();

            if (finalToken.Is(tokenizer::SpecialPurpose::EndOfFile) ||
                (std::holds_alternative<tokenizer::Token::Type>(m_Token) &&
                 std::get<tokenizer::Token::Type>(m_Token) ==
                         finalToken.GetValueType()))
                m_Token = finalToken.GetValueType();
            else
                m_Token = std::move(token);

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

        static Super Until(tokenizer::Token::Type untilType) {
            return Super{untilType};
        }

        static Super Until(UntilCondition const &untilCondition) {
            return Super{untilCondition};
        }

        [[nodiscard]]
        bool operator==(Super const &other) const noexcept {
            if (std::holds_alternative<UntilCondition>(other.m_Token)) {
                if (!std::holds_alternative<ValueType>(m_Token))
                    return true;// both are special purpose

                auto const condition{std::get<UntilCondition>(other.m_Token)};

                return condition(std::get<ValueType>(m_Token));
            }

            if (std::holds_alternative<tokenizer::Token::Type>(other.m_Token)) {
                if (!std::holds_alternative<ValueType>(m_Token))
                    return true;// both are special purpose

                auto const currentTokenType{
                        internal::GetToken(std::get<ValueType>(m_Token))
                                .GetValueType()
                };
                auto const isCurrentEOF{
                        std::holds_alternative<tokenizer::SpecialPurpose>(
                                currentTokenType
                        ) &&
                        std::get<tokenizer::SpecialPurpose>(currentTokenType) ==
                                tokenizer::SpecialPurpose::EndOfFile
                };

                return isCurrentEOF ||
                       currentTokenType ==
                               std::get<tokenizer::Token::Type>(other.m_Token);
            }

            return std::get<ValueType>(m_Token) ==
                   std::get<ValueType>(other.m_Token);
        }

        [[nodiscard]]
        bool operator!=(Super const &other) const noexcept {
            return !(*this == other);
        }
    };

    template<class T>
    concept IsPreprocessorIterator = requires(T t) {
        requires std::input_iterator<T> && std::default_initializable<T>;
        { T::end() } -> std::same_as<T>;
        { T::Until(tokenizer::SpecialPurpose::EndOfFile) } -> std::same_as<T>;
        {
            T::Until([](tokenizer::Token const &) { return false; })
        } -> std::same_as<T>;
        { t.GetNextToken() } -> std::same_as<tokenizer::Token>;
    };

    class PreprocessorIterator final
        : public BasePreprocessorIterator<PreprocessorIterator> {
    public:
        using BasePreprocessorIterator::BasePreprocessorIterator;

        [[nodiscard]]
        tokenizer::Token GetNextToken() const;
    };

    static_assert(IsPreprocessorIterator<PreprocessorIterator>);

    class PreprocessorIteratorNoCommands final
        : public BasePreprocessorIterator<PreprocessorIteratorNoCommands> {
    public:
        using BasePreprocessorIterator::BasePreprocessorIterator;

        [[nodiscard]]
        tokenizer::Token GetNextToken() const;
    };

    static_assert(IsPreprocessorIterator<PreprocessorIteratorNoCommands>);

    class PreprocessorIteratorVerbatim final
        : public BasePreprocessorIterator<
                  PreprocessorIteratorVerbatim, PreprocessorToken> {
    public:
        using BasePreprocessorIterator::BasePreprocessorIterator;

        [[nodiscard]]
        PreprocessorToken GetNextToken() const;
    };

    class InternalPreprocessorIterator final {
        mutable Preprocessor *m_pPreprocessor{};
        mutable std::variant<tokenizer::Token::Type, PreprocessorToken> m_Token{
                tokenizer::SpecialPurpose::EndOfFile
        };

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = PreprocessorToken;
        using reference         = value_type &;
        using pointer           = value_type *;
        using difference_type   = int;

        InternalPreprocessorIterator();

        explicit InternalPreprocessorIterator(Preprocessor &preprocessor);

        explicit InternalPreprocessorIterator(tokenizer::Token::Type untilType);

        static InternalPreprocessorIterator
        Until(tokenizer::Token::Type untilType);

        reference operator*() const;

        pointer operator->() const;

        InternalPreprocessorIterator &operator++();

        [[nodiscard]]
        InternalPreprocessorIterator operator++(int);

        InternalPreprocessorIterator begin() const;

        static InternalPreprocessorIterator end();

        [[nodiscard]]
        bool operator==(InternalPreprocessorIterator const &other
        ) const noexcept;

        [[nodiscard]]
        bool operator!=(InternalPreprocessorIterator const &other
        ) const noexcept;
    };

    static_assert(std::input_iterator<InternalPreprocessorIterator>);
}// namespace jcc::preprocessor

#endif//PREPROCESSOR_ITERATOR_H
