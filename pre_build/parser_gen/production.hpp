#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <ostream>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

namespace jcc::parser_gen {
    struct NonTerminal final {
        std::string m_Name;

        static NonTerminal const c_SPrime;

        bool operator==(NonTerminal const &other) const;
    };

    struct Terminal final {
        std::string m_Token{};

        static Terminal const c_Epsilon;
        static Terminal const c_Eof;

        [[nodiscard]]
        bool
        operator==(Terminal const &other) const;

        [[nodiscard]]
        bool IsEpsilon() const;
    };

    struct NonTerminalHash final {
        [[nodiscard]]
        std::size_t
        operator()(NonTerminal const &nonTerminal) const;
    };

    struct TerminalHash final {
        [[nodiscard]]
        std::size_t
        operator()(Terminal const &terminal) const;
    };

    struct TerminalIndex final {
        std::size_t m_Index{};

        static TerminalIndex const c_EpsilonIndex;

        static TerminalIndex const c_EofIndex;

        [[nodiscard]]
        bool
        operator==(TerminalIndex const &other) const;
    };

    class TerminalIndexHash final {
    public:
        [[nodiscard]]
        std::size_t
        operator()(TerminalIndex const &terminalIndex) const;
    };

    struct NonTerminalIndex final {
        std::size_t m_Index{};

        static NonTerminalIndex const c_SPrimeIndex;

        [[nodiscard]]
        bool
        operator==(NonTerminalIndex const &other) const;
    };

    class NonTerminalIndexHash final {
    public:
        [[nodiscard]]
        std::size_t
        operator()(NonTerminalIndex const &nonTerminalIndex) const;
    };

    using Symbol =
            std::variant<TerminalIndex, NonTerminalIndex, std::nullptr_t>;

    class SymbolHash final {
    public:
        [[nodiscard]]
        std::size_t
        operator()(Symbol const &symbol) const;
    };

    [[nodiscard]]
    std::string const &GetSymbolName(
            std::vector<NonTerminal> const &nonTerminals,
            std::vector<Terminal> const &terminals, Symbol const &symbol
    );

    struct Production final {
        NonTerminalIndex    m_NonTerminal;
        std::vector<Symbol> m_Symbols;
        int                 m_Index{};
    };

    using TerminalSet = std::unordered_set<TerminalIndex, TerminalIndexHash>;

    struct Item final {
        Production const *m_Production{};
        int               m_Position{};
        TerminalSet       m_LookAhead{};

        [[nodiscard]]
        bool
        operator==(Item const &other) const;

        [[nodiscard]]
        Symbol GetSymbolAtPosition(std::size_t offset = 0) const;

        [[nodiscard]]
        std::unordered_set<Symbol, SymbolHash> GetSymbolsAfterPosition() const;

        [[nodiscard]]
        bool HasNextSymbol() const;
    };

    // hash of Item
    struct ItemHash final {
        [[nodiscard]]
        std::size_t
        operator()(Item const &item) const;
    };

    using ItemSet = std::unordered_set<Item, ItemHash>;

    struct ItemSetHash final {
        [[nodiscard]]
        std::size_t
        operator()(ItemSet const &itemSet) const;
    };
}// namespace jcc::parser_gen

#endif//PRODUCTION_H
