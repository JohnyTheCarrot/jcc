#include "cast_expression.hpp"

#include <diagnostics/variants/parsing/basic_syntax_error.hpp>

#include "diagnostics/variants/sema/invalid_specifier_qualifier_list.hpp"
#include "expression.h"
#include "postfix_expression.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing_sema {
    AstCastExpression::AstCastExpression(
            Span &&span, AstTypeName type, AstExpressionPtr &&expression
    )
        : AstExpression{std::move(span), types::ValueType{type.GetType()}}
        , m_TypeName{std::move(type)}
        , m_Expression{std::move(expression)} {
    }

    void
    AstCastExpression::AcceptOnExpression(ExpressionVisitor *visitor) const {
        visitor->Visit(this);
    }

    AstExpressionPtr ParseCastExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        if (current == end)
            return nullptr;

        Span span{current->m_Span};

        if (!current->Is(tokenizer::Punctuator::LeftParenthesis)) {
            // TODO: should be unary-expression
            return ParsePostfixExpression(current, end);
        }
        ++current;
        if (auto typeName{ParseTypeName(current, end)}; typeName.has_value()) {
            if (current == end ||
                !current->Is(tokenizer::Punctuator::RightParenthesis)) {
                CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<diagnostics::BasicSyntaxError>(
                                typeName->m_Span.m_Source,
                                typeName->m_Span.m_Span,
                                "Expected this to be followed by a closing "
                                "parenthesis"
                        );
            }

            if (!typeName->GetSpecifierQualifierList().IsValid()) {
                CompilerState::GetInstance()
                        .EmplaceFatalDiagnostic<
                                diagnostics::InvalidSpecifierQualifierList>(
                                typeName->m_Span.m_Source,
                                typeName->m_Span.m_Span
                        );
            }

            ++current;
            auto expression{ParseCastExpression(current, end)};

            return std::make_unique<AstCastExpression>(
                    span + typeName->m_Span + expression->m_Span,
                    std::move(typeName.value()), std::move(expression)
            );
        }

        // We're potentially dealing with a primary expression
        auto expressionPtr{ParseExpression(current, end)};

        if (current == end) {
            CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<diagnostics::BasicSyntaxError>(
                            std::move(expressionPtr->m_Span.m_Source),
                            expressionPtr->m_Span.m_Span,
                            "Expected this to be followed by a closing "
                            "parenthesis"
                    );
        }

        if (!current->Is(tokenizer::Punctuator::RightParenthesis)) {
            CompilerState::GetInstance()
                    .EmplaceFatalDiagnostic<diagnostics::BasicSyntaxError>(
                            std::move(current->m_Span.m_Source),
                            current->m_Span.m_Span, "a closing parenthesis",
                            current->ToString()
                    );
        }

        return expressionPtr;
    }
}// namespace jcc::parsing_sema
