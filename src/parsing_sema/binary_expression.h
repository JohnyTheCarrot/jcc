#ifndef BINARY_EXPRESSION_H
#define BINARY_EXPRESSION_H
#include "ast_node.h"
#include "misc/Diagnosis.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::tokenizer {
    enum class Punctuator;
}

namespace jcc::parsing_sema {
    namespace internal {
        using ParseChildFn = std::function<
                AstExpressionPtr(preprocessor::PreprocessorIterator &, preprocessor::PreprocessorIterator const &)>;
    }// namespace internal

    template<typename Op, typename Node>
        requires std::is_constructible_v<
                Node, AstExpressionPtr, AstExpressionPtr, Op, mjolnir::Span>
    [[nodiscard]]
    AstExpressionPtr ParseBinaryExpression(
            std::function<std::optional<Op>(tokenizer::Punctuator)> const &opFn,
            internal::ParseChildFn const             &parseChild,
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        auto lhsExpr{parseChild(current, end)};

        if (lhsExpr == nullptr)
            return nullptr;

        while (current != end) {
            if (tokenizer::Token nextToken{*current};
                nextToken.Is<tokenizer::Punctuator>()) {

                auto const op{
                        opFn(std::get<tokenizer::Punctuator>(nextToken.m_Value))
                };

                if (!op.has_value())
                    return lhsExpr;

                ++current;
                if (current == end) {
                    // TODO: Diagnosis
                    throw FatalCompilerError{
                            // Diagnosis::Kind::PRS_ExpectedExpressionToFollow,
                            // std::move(nextToken.m_Span)
                    };
                }

                auto rhsExpr{parseChild(current, end)};
                if (rhsExpr == nullptr) {
                    // TODO: Diagnosis
                    throw FatalCompilerError{
                            // Diagnosis::Kind::PRS_ExpectedExpressionToFollow,
                            // std::move(nextToken.m_Span)
                    };
                }

                lhsExpr = std::make_unique<Node>(
                        std::move(lhsExpr), std::move(rhsExpr), op.value(),
                        nextToken.m_Span.m_Span
                );

                continue;
            }

            return lhsExpr;
        }

        return lhsExpr;
    }
}// namespace jcc::parsing_sema

#endif//BINARY_EXPRESSION_H
