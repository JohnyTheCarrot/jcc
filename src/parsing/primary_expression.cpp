#include "primary_expression.h"

#include <diagnostics/variants/parsing/basic_syntax_error.hpp>

#include "constant.h"
#include "diagnostics/variants/todo.hpp"
#include "expression.h"

namespace jcc::parsing {
    AstExpressionPtr ParsePrimaryExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        auto firstToken{*current};
        ++current;
        auto &compilerState{CompilerState::GetInstance()};

        if (firstToken.Is(tokenizer::Punctuator::LeftParenthesis)) {
            auto result{ParseExpression(current, end)};
            if (current == end ||
                !current->Is(tokenizer::Punctuator::RightParenthesis)) {
                compilerState
                        .EmplaceFatalDiagnostic<diagnostics::BasicSyntaxError>(
                                std::move(firstToken.m_Span.m_Source),
                                firstToken.m_Span.m_Span,
                                "a closing parenthesis", current->ToString()
                        );
            }
            result->m_Span = firstToken.m_Span + current->m_Span;
            ++current;

            return result;
        }

        if (firstToken.Is(tokenizer::Keyword::Generic)) {
            // TODO: Generic selection
            compilerState.EmplaceFatalDiagnostic<diagnostics::TodoError>(
                    std::move(firstToken.m_Span.m_Source),
                    firstToken.m_Span.m_Span
            );
        }

        if (firstToken.Is<tokenizer::Identifier>()) {
            // TODO: Identifier
            compilerState.EmplaceFatalDiagnostic<diagnostics::TodoError>(
                    std::move(firstToken.m_Span.m_Source),
                    firstToken.m_Span.m_Span
            );
        }

        if (firstToken.Is<tokenizer::StringConstant>()) {
            // TODO: String literal
            compilerState.EmplaceFatalDiagnostic<diagnostics::TodoError>(
                    std::move(firstToken.m_Span.m_Source),
                    firstToken.m_Span.m_Span
            );
        }

        // May return nullptr, which is fine, it just means there's no syntactical match.
        return ParseConstant(firstToken);
    }
}// namespace jcc::parsing
