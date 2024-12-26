#ifndef PARSER_STATE_H
#define PARSER_STATE_H

#include "ast_node.h"
#include "misc/Diagnosis.h"
#include "tokenizer/token.h"

namespace jcc::parsing_sema {
    template<typename TIterator>
        requires std::input_iterator<TIterator> and
                 std::same_as<std::iter_value_t<TIterator>, tokenizer::Token>
    class Parser {
        TIterator m_Current;
        TIterator m_End;

    protected:
        void Advance() {
            if (m_Current != m_End) {
                ++m_Current;
            }
        }

        [[nodiscard]]
        TIterator GetCurrent() const {
            return m_Current;
        }

        [[nodiscard]]
        TIterator GetEnd() const {
            return m_End;
        }

        [[nodiscard]]
        bool IsExhausted() const {
            return m_Current == m_End;
        }

        [[nodiscard]]
        int GetDistance() const {
            return std::distance(m_Current, m_End);
        }

    public:
        using NodePtr = std::unique_ptr<AstNode>;

        Parser(TIterator begin, TIterator end)
            : m_Current{begin}
            , m_End{end} {
        }
    };
}// namespace jcc::parsing_sema

#endif//PARSER_STATE_H
