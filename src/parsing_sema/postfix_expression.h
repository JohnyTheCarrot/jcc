#ifndef POSTFIX_EXPRESSION_H
#define POSTFIX_EXPRESSION_H

#include "ast_node.h"
#include "misc/Diagnosis.h"
#include "parser.h"
#include "primary_expression.h"
#include "types.h"

namespace jcc::tokenizer {
    struct Token;
}

namespace jcc::parsing_sema {
    template<typename It>
        requires TokenIterator<It>
    [[nodiscard]]
    AstExpressionPtr ParsePostfixExpression(It current, It end) {
        if (current == end)
            return nullptr;

        auto &firstToken{*current};

        // TODO: if firstToken is a left paren, it may be a compound literal
        auto primaryExpr{ParsePrimaryExpression(firstToken)};

        ++current;
        if (current == end)
            return primaryExpr;

        // TODO: while next is one of [, (, ., ->, ++ or --, parse the appropriate postfix operator
        if (tokenizer::Token & nextToken{*current};
            nextToken.Is<tokenizer::Punctuator>()) {
            switch (std::get<tokenizer::Punctuator>(nextToken.m_Value)) {
                case tokenizer::Punctuator::LeftBracket:
                case tokenizer::Punctuator::LeftParenthesis:
                case tokenizer::Punctuator::Dot:
                case tokenizer::Punctuator::Arrow:
                case tokenizer::Punctuator::PlusPlus:
                case tokenizer::Punctuator::MinusMinus:
                    // TODO: diagnosis
                    throw FatalCompilerError{
                            // Diagnosis::Kind::TODO, std::move(nextToken.m_Span)
                    };
                default:
                    break;
            }
        }

        return primaryExpr;
    }
}// namespace jcc::parsing_sema
#endif//POSTFIX_EXPRESSION_H
