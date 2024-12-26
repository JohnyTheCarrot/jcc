#ifndef INTEGER_CONSTANT_H
#define INTEGER_CONSTANT_H

#include <cstdint>

#include "ast_node.h"
#include "parser.h"
#include "tokenizer/token.h"
#include "types/type.h"

namespace jcc::parsing_sema {
    class IntegerConstant final : AstNode {
        types::IntegerType m_Type;
        std::int64_t       m_Value;

    public:
        IntegerConstant(types::IntegerType type, std::int64_t value);
    };

    struct IntegerSuffix final {
        types::IntegerType::Signedness          m_Sign;
        std::optional<types::IntegerType::Type> m_Type;

        [[nodiscard]]
        bool operator==(IntegerSuffix const &other) const = default;
    };

    void PrintTo(IntegerSuffix const &suffix, std::ostream *os);

    template<typename TIterator>
        requires std::input_iterator<TIterator> and
                 std::same_as<std::iter_value_t<TIterator>, tokenizer::Token>
    class IntegerConstantParser final : public Parser<TIterator> {
    public:
        [[nodiscard]]
        std::optional<IntegerSuffix> ParseIntegerSuffix(
                Span const &span, std::string::const_iterator &cIt,
                std::string::const_iterator end
        ) const {
            using Signedness = types::IntegerType::Signedness;

            if (cIt == end) {
                return std::nullopt;
            }

            Span suffixSpan{span};
            suffixSpan.m_Start =
                    span.m_End -
                    (static_cast<int>(std::distance(cIt, end)) - 1);

            auto const parseLongSuffix = [&cIt, end, &suffixSpan] {
                auto const current{*cIt};
                auto const nextIt{std::next(cIt)};

                if (nextIt == end || *nextIt == 'u') {
                    return types::StandardIntegerType::Long;
                }

                ++cIt;

                if (current != *cIt) {
                    throw FatalCompilerError{
                            Diagnosis::Kind::PRS_UnrecognizedIntegerSuffix,
                            std::move(suffixSpan)
                    };
                }

                return types::StandardIntegerType::LongLong;
            };

            auto const parseBitPreciseIntSuffix = [&cIt, end, &suffixSpan] {
                bool const wasFirstLower{std::islower(*cIt) != 0};

                ++cIt;
                if (cIt == end && *cIt != 'b' && *cIt != 'B' &&
                    std::islower(*cIt) != wasFirstLower) {
                    throw FatalCompilerError{
                            Diagnosis::Kind::PRS_UnrecognizedIntegerSuffix,
                            std::move(suffixSpan)
                    };
                }

                // TODO: figure out the minimum number of bits required to represent the integer
                return types::BitInteger{1};
            };

            auto const parseTypeSuffix =
                    [&] -> std::optional<types::IntegerType::Type> {
                if (cIt == end) {
                    return std::nullopt;
                }

                switch (std::tolower(*cIt)) {
                    case 'l':
                        return parseLongSuffix();
                    case 'w':
                        return parseBitPreciseIntSuffix();
                    default:
                        throw FatalCompilerError{
                                Diagnosis::Kind::PRS_UnrecognizedIntegerSuffix,
                                std::move(suffixSpan)
                        };
                }
            };

            auto sign{Signedness::Unspecified};
            std::optional<types::IntegerType::Type> type{};

            switch (std::tolower(*cIt)) {
                case 'u':
                    sign = Signedness::Unsigned;
                    ++cIt;
                    type = parseTypeSuffix();
                    break;
                case 'l':
                case 'w':
                    type = parseTypeSuffix();
                    if (auto const next{std::next(cIt)}; next != end) {
                        if (std::tolower(*next) != 'u') {
                            throw FatalCompilerError{
                                    Diagnosis::Kind::
                                            PRS_UnrecognizedIntegerSuffix,
                                    std::move(suffixSpan)
                            };
                        }
                        sign = Signedness::Unsigned;
                        ++cIt;
                    }
                    break;
                default:
                    throw FatalCompilerError{
                            Diagnosis::Kind::PRS_UnrecognizedIntegerSuffix,
                            std::move(suffixSpan)
                    };
            }

            return IntegerSuffix{sign, type};
        }

        using Parser<TIterator>::Parser;

        [[nodiscard]]
        typename IntegerConstantParser::NodePtr ParseIntegerConstant() {
            if (this->IsExhausted()) {
                return nullptr;
            }

            auto               &token{*this->GetCurrent()};
            tokenizer::PpNumber ppNumber{
                    std::get<tokenizer::PpNumber>(token.m_Value)
            };

            for (auto c : ppNumber.m_Number) {}
        }
    };
}// namespace jcc::parsing_sema

#endif//INTEGER_CONSTANT_H
