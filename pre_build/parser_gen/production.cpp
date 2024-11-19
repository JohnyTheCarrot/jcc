#include "production.hpp"

#include <algorithm>
#include <iterator>

namespace jcc::parser_gen {
    std::string const c_Epsilon{"ε"};

    bool Terminal::operator==(Terminal const &other) const {
        return m_Token == other.m_Token;
    }

    Terminal const Terminal::c_Epsilon{"<Epsilon>"};

    std::size_t TerminalHash::operator()(Terminal const &terminal) const {
        return std::hash<std::string>{}(terminal.m_Token);
    }

    std::string const &GetSymbolName(Symbol const &symbol) {
        if (std::holds_alternative<std::nullptr_t>(symbol)) {
            return c_Epsilon;
        }

        if (std::holds_alternative<NonTerminal const *>(symbol)) {
            auto const *terminal{std::get<NonTerminal const *>(symbol)};
            if (terminal == nullptr) {
                return c_Epsilon;
            }

            return std::get<NonTerminal const *>(symbol)->m_Name;
        }

        return std::get<Terminal const *>(symbol)->m_Token;
    }

    std::ostream &operator<<(std::ostream &os, Production const &prod) {
        if (prod.m_Terminal == nullptr) {
            os << 'T';
        } else {
            os << prod.m_Terminal->m_Name;
        }
        os << " -> ";

        std::ranges::transform(
                prod.m_Symbols, std::ostream_iterator<std::string>{os, " "},
                GetSymbolName
        );

        return os;
    }

    bool Item::operator==(Item const &other) const {
        return m_Production == other.m_Production &&
               m_Position == other.m_Position;
    }

    std::size_t ItemHash::operator()(Item const &item) const {
        return std::hash<Production const *>{}(item.m_Production) ^
               std::hash<int>{}(item.m_Position);
    }

    std::ostream &operator<<(std::ostream &os, Item const &item) {
        auto const &[prodSymbol, symbols]{*item.m_Production};

        if (prodSymbol == nullptr) {
            os << 'T';
        } else {
            os << prodSymbol->m_Name;
        }
        os << " -> ";

        std::transform(
                symbols.cbegin(), symbols.cbegin() + item.m_Position,
                std::ostream_iterator<std::string>{os, " "}, GetSymbolName
        );

        os << "∘ ";

        std::transform(
                symbols.cbegin() + item.m_Position, symbols.cend(),
                std::ostream_iterator<std::string>{os, " "}, GetSymbolName
        );

        return os;
    }

    std::ostream &operator<<(std::ostream &os, ItemSet const &itemSet) {
        std::ranges::copy(itemSet, std::ostream_iterator<Item>{os, "\n"});

        return os;
    }
}// namespace jcc::parser_gen
