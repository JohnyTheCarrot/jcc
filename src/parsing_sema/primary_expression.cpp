#include "primary_expression.h"

#include "constant.h"
#include "expression.h"
#include "misc/Diagnosis.h"

namespace jcc::parsing_sema {
    AstExpressionPtr ParsePrimaryExpression(
            preprocessor::PreprocessorIterator       &current,
            preprocessor::PreprocessorIterator const &end
    ) {
        auto firstToken{*current};
        ++current;

        if (firstToken.Is(tokenizer::Punctuator::LeftParenthesis)) {
            auto result{ParseExpression(current, end)};
            if (current == end ||
                !current->Is(tokenizer::Punctuator::RightParenthesis)) {
                throw FatalCompilerError{
                        Diagnosis::Kind::PRS_ExpectedRParen,
                        std::move(firstToken.m_Span)
                };
            }
            result->m_Span = firstToken.m_Span + current->m_Span;
            ++current;

            return result;
        }

        if (firstToken.Is(tokenizer::Keyword::Generic)) {
            // TODO: Generic selection
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(firstToken.m_Span)
            };
        }

        if (firstToken.Is<tokenizer::Identifier>()) {
            // TODO: Identifier
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(firstToken.m_Span)
            };
        }

        if (firstToken.Is<tokenizer::StringConstant>()) {
            // TODO: String literal
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(firstToken.m_Span)
            };
        }

        // May return nullptr, which is fine, it just means there's no syntactical match.
        return ParseConstant(firstToken);
    }
}// namespace jcc::parsing_sema
