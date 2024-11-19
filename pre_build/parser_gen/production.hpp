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

    struct TerminalHash final {
        [[nodiscard]]
        std::size_t
        operator()(Terminal const &terminal) const;
    };

    using Symbol =
            std::variant<Terminal const *, NonTerminal const *, std::nullptr_t>;

    [[nodiscard]]
    std::string const &GetSymbolName(Symbol const &symbol);

    struct Production final {
        NonTerminal const  *m_Terminal;
        std::vector<Symbol> m_Symbols;
    };

    std::ostream &operator<<(std::ostream &os, Production const &prod);

    struct Item final {
        Production const *m_Production{};
        int               m_Position{};

        [[nodiscard]]
        bool
        operator==(Item const &other) const;

        [[nodiscard]]
        Symbol GetSymbolAtPosition() const;

        [[nodiscard]]
        bool IsKernel() const;
    };

    // hash of Item
    struct ItemHash final {
        [[nodiscard]]
        std::size_t
        operator()(Item const &item) const;
    };

    std::ostream &operator<<(std::ostream &os, Item const &item);

    using ItemSet = std::unordered_set<Item, ItemHash>;

    std::ostream &operator<<(std::ostream &os, ItemSet const &itemSet);
}// namespace jcc::parser_gen

#endif//PRODUCTION_H
