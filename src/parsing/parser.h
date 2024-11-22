#ifndef PARSER_H
#define PARSER_H

#include <deque>
#include <stack>

#include "ir_codegen/target/common.h"
#include "lr_table.hpp"
#include "lrtable_utils.hpp"
#include "preprocessor/preprocessor.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing {
    class Parser final {
        using StackToken = std::variant<tokenizer::Token, NonTerminal>;

        std::deque<codegen::BinaryOutput>  m_Output;
        std::stack<State>                  m_States;
        std::stack<StackToken>             m_NonTerminals;
        preprocessor::PreprocessorIterator m_CurrentIt;
        preprocessor::Preprocessor        *m_Preprocessor;

        [[nodiscard]]
        ActionRowElement GetAction(tokenizer::Token const &token);

    public:
        explicit Parser(preprocessor::Preprocessor &preprocessor);

        [[nodiscard]]
        codegen::BinaryOutput ReadOutput();
    };
}// namespace jcc::parsing

#endif//PARSER_H
