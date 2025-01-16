#include "primary_expression.h"

#include "constant.h"
#include "misc/Diagnosis.h"
#include "tokenizer/token.h"

namespace jcc::parsing_sema {
    AstExpressionPtr ParsePrimaryExpression(tokenizer::Token &token) {
        if (token.Is(tokenizer::Punctuator::LeftParenthesis)) {
            // TODO: Parenthesized expression
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(token.m_Span)
            };
        }

        if (token.Is(tokenizer::Keyword::Generic)) {
            // TODO: Generic selection
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(token.m_Span)
            };
        }

        if (token.Is<tokenizer::Identifier>()) {
            // TODO: Identifier
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(token.m_Span)
            };
        }

        if (token.Is<tokenizer::StringConstant>()) {
            // TODO: String literal
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(token.m_Span)
            };
        }

        // May return nullptr, which is fine, it just means there's no syntactical match.
        return ParseConstant(token);
    }
}// namespace jcc::parsing_sema
