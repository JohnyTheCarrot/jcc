#ifndef PARSER_H
#define PARSER_H
#include <deque>
#include <stack>

#include "ir_codegen/target/common.h"
#include "preprocessor/preprocessor.h"
#include "preprocessor/preprocessor_iterator.h"

namespace jcc::parsing {
    enum class NonTerminal { E, T, F, Max };

    using Grammar = std::array<std::pair<NonTerminal, int>, 6>;

    constexpr Grammar c_Grammar{
            std::pair{NonTerminal::E, 3}, {NonTerminal::E, 1},
            {NonTerminal::T, 3},          {NonTerminal::T, 1},
            {NonTerminal::F, 3},          {NonTerminal::F, 1}
    };

    struct ActionRowElement final {
        enum class Action {
            Shift,
            Reduce,
            Accept,
            Error,
        };

        int    state;
        Action action;
    };

    consteval ActionRowElement Accept() {
        return {0, ActionRowElement::Action::Accept};
    }

    consteval ActionRowElement Error() {
        return {0, ActionRowElement::Action::Error};
    }

    consteval ActionRowElement Reduce(int state) {
        return {state, ActionRowElement::Action::Reduce};
    }

    consteval ActionRowElement Shift(int state) {
        return {state, ActionRowElement::Action::Shift};
    }

    using State = int;

    using ActionRow =
            std::array<ActionRowElement, tokenizer::Token::c_NumTokenTypes>;

    using GotoRow = std::array<
            std::optional<State>, static_cast<std::size_t>(NonTerminal::Max)>;

    using LrOneTable = std::array<std::pair<ActionRow, GotoRow>, 12>;

    constexpr LrOneTable c_LrOneTable{[] {
        LrOneTable table{};

        table[0] = std::make_pair<ActionRow, GotoRow>(
                {Shift(5), Error(), Error(), Shift(4), Error(), Error()},
                GotoRow{1, 2, 3}
        );
        table[1] = std::make_pair<ActionRow, GotoRow>(
                {Error(), Shift(6), Error(), Error(), Error(), Accept()},
                GotoRow{}
        );
        table[2] = std::make_pair<ActionRow, GotoRow>(
                {Error(), Reduce(2), Shift(7), Error(), Reduce(2), Reduce(2)},
                GotoRow{}
        );
        table[3] = std::make_pair<ActionRow, GotoRow>(
                {Error(), Reduce(4), Reduce(4), Error(), Reduce(4), Reduce(4)},
                GotoRow{}
        );
        table[4] = std::make_pair<ActionRow, GotoRow>(
                {Shift(5), Error(), Error(), Shift(4), Error(), Error()},
                GotoRow{8, 2, 3}
        );
        table[5] = std::make_pair<ActionRow, GotoRow>(
                {Error(), Reduce(6), Reduce(6), Error(), Reduce(6), Reduce(6)},
                GotoRow{}
        );
        table[6] = std::make_pair<ActionRow, GotoRow>(
                {Shift(5), Error(), Error(), Shift(4), Error(), Error()},
                GotoRow{std::nullopt, 9, 3}
        );
        table[7] = std::make_pair<ActionRow, GotoRow>(
                {Shift(5), Error(), Error(), Shift(4), Error(), Error()},
                GotoRow{std::nullopt, std::nullopt, 10}
        );
        table[8] = std::make_pair<ActionRow, GotoRow>(
                {Error(), Shift(6), Error(), Error(), Shift(11), Error()},
                GotoRow{}
        );
        table[9] = std::make_pair<ActionRow, GotoRow>(
                {Error(), Reduce(1), Shift(7), Error(), Reduce(1), Reduce(1)},
                GotoRow{}
        );
        table[10] = std::make_pair<ActionRow, GotoRow>(
                {Error(), Reduce(3), Reduce(3), Error(), Reduce(3), Reduce(3)},
                GotoRow{}
        );
        table[11] = std::make_pair<ActionRow, GotoRow>(
                {Error(), Reduce(5), Reduce(5), Error(), Reduce(5), Reduce(5)},
                GotoRow{}
        );

        return table;
    }()};

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
