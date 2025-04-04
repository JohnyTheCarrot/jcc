#include "primary_expression.h"

#include <diagnostics/variants/parsing/basic_syntax_error.hpp>// for Basic...
#include <memory>                                             // for uniqu...
#include <utility>                                            // for move

#include "constant.h"                          // for Parse...
#include "diagnostics/variants/todo.hpp"       // for TodoE...
#include "expression.h"                        // for Parse...
#include "misc/Span.h"                         // for Span
#include "parsing/ast_node.h"                  // for AstEx...
#include "parsing/parser.h"                    // for Compi...
#include "preprocessor/preprocessor_iterator.h"// for BaseP...
#include "tokenizer/token.h"                   // for Ident...

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
