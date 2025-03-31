#ifndef BINARY_EXPRESSION_H
#define BINARY_EXPRESSION_H

#include "ast_node.h"
#include "diagnostics/variants/parsing/expected_expression.hpp"
#include "parsing/parser.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::tokenizer {
    enum class Punctuator;
}

namespace jcc::parsing {
    namespace internal {
        using ParseChildFn = std::function<
                AstExpressionPtr(preprocessor::PreprocessorIterator &, preprocessor::PreprocessorIterator const &)>;
    }// namespace internal

    /**
     * Parses a binary expression.
     * 
     * @tparam Op           The operator enum class
     * @tparam Node         The AST node type
     * @param opMapFn       A function that maps a punctuator to an operator
     * @param parseChild    A function that parses a child expression
     * @param current       The current iterator
     * @param end           The end iterator
     * @return              The parsed binary expression
     */
    template<typename Op, typename Node>
        requires std::is_constructible_v<
                Node, AstExpressionPtr, AstExpressionPtr, Op, mjolnir::Span>
    [[nodiscard]]
    AstExpressionPtr ParseBinaryExpression(
            std::function<std::optional<Op>(tokenizer::Punctuator)> const
                                                     &opMapFn,
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

                auto const op{opMapFn(
                        std::get<tokenizer::Punctuator>(nextToken.m_Value)
                )};

                if (!op.has_value())
                    return lhsExpr;

                ++current;
                if (current == end) {
                    CompilerState::GetInstance()
                            .EmplaceFatalDiagnostic<
                                    diagnostics::ExpectedExpression>(
                                    std::move(nextToken.m_Span.m_Source),
                                    lhsExpr->m_Span.m_Span,
                                    nextToken.m_Span.m_Span, std::nullopt
                            );
                }

                auto firstRhsToken{*current};
                auto rhsExpr{parseChild(current, end)};
                if (rhsExpr == nullptr) {
                    CompilerState::GetInstance()
                            .EmplaceFatalDiagnostic<
                                    diagnostics::ExpectedExpression>(
                                    std::move(nextToken.m_Span.m_Source),
                                    lhsExpr->m_Span.m_Span,
                                    nextToken.m_Span.m_Span,
                                    firstRhsToken.m_Span.m_Span
                            );
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
}// namespace jcc::parsing

#endif//BINARY_EXPRESSION_H
