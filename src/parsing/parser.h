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
    struct NonTerminalAstNode;

    using AstNode = std::variant<tokenizer::Token, NonTerminalAstNode>;

    struct NonTerminalAstNode final {
        NonTerminal          m_Type{};
        std::vector<AstNode> m_Children{};
        NonTerminalAstNode  *m_Parent{nullptr};
    };

    void PrintTo(NonTerminalAstNode const &node, int depth, std::ostream &os);

    void PrintTo(AstNode const &node, int depth, std::ostream &os);

    class Parser final {
        std::stack<State>                  m_States;
        std::stack<AstNode>                m_NonTerminals;
        preprocessor::PreprocessorIterator m_CurrentIt;
        preprocessor::Preprocessor        *m_Preprocessor;

        [[nodiscard]]
        ActionRowElement GetAction(tokenizer::Token const &token);

    public:
        explicit Parser(preprocessor::Preprocessor &preprocessor);

        [[nodiscard]]
        bool ReadOutput(AstNode &ast);
    };
}// namespace jcc::parsing

#endif//PARSER_H
