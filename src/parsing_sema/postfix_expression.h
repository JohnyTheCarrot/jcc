#ifndef POSTFIX_EXPRESSION_H
#define POSTFIX_EXPRESSION_H

#include "ast_node.h"
#include "misc/Diagnosis.h"
#include "parser.h"
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

        auto const &firstToken{*current};

        if (firstToken.Is(tokenizer::Punctuator::LeftParenthesis)) {
            ++current;
            if (current == end) {
                throw FatalCompilerError{
                        Diagnosis::Kind::PRS_SyntaxError,
                        std::move(firstToken.m_Span),
                        "Expected either an expression or a storage class "
                        "specifier."
                };
            }

            if (specifierParseStorageClassSpecifier(*current)) {}
        }
    }
}// namespace jcc::parsing_sema
#endif//POSTFIX_EXPRESSION_H
