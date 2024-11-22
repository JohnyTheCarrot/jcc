#include "production.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>

namespace jcc::parser_gen {
    std::string const c_Epsilon{"ε"};
    std::string const c_SPrime{"S'"};

    bool NonTerminal::operator==(NonTerminal const &other) const {
        return m_Name == other.m_Name;
    }

    bool Terminal::operator==(Terminal const &other) const {
        return m_Token == other.m_Token;
    }

    bool Terminal::IsEpsilon() const {
        return m_Token == c_Epsilon.m_Token;
    }

    std::size_t NonTerminalHash::operator()(NonTerminal const &nonTerminal
    ) const {
        return std::hash<std::string>{}(nonTerminal.m_Name);
    }

    NonTerminal const NonTerminal::c_SPrime{"SPrime"};

    Terminal const Terminal::c_Epsilon{"ε"};
    Terminal const Terminal::c_Eof{"jcc::tokenizer::SpecialPurpose::EndOfFile"};

    std::size_t TerminalHash::operator()(Terminal const &terminal) const {
        return std::hash<std::string>{}(terminal.m_Token);
    }

    TerminalIndex const TerminalIndex::c_EpsilonIndex{
            std::numeric_limits<std::size_t>::max()
    };

    TerminalIndex const TerminalIndex::c_EofIndex{0};

    bool TerminalIndex::operator==(TerminalIndex const &other) const {
        return m_Index == other.m_Index;
    }

    std::size_t TerminalIndexHash::operator()(TerminalIndex const &terminalIndex
    ) const {
        return terminalIndex.m_Index;
    }

    bool NonTerminalIndex::operator==(NonTerminalIndex const &other) const {
        return m_Index == other.m_Index;
    }

    NonTerminalIndex const NonTerminalIndex::c_SPrimeIndex{0};

    std::size_t
    NonTerminalIndexHash::operator()(NonTerminalIndex const &nonTerminalIndex
    ) const {
        return nonTerminalIndex.m_Index;
    }

    std::size_t SymbolHash::operator()(Symbol const &symbol) const {
        if (std::holds_alternative<std::nullptr_t>(symbol)) {
            return 0;
        }

        if (std::holds_alternative<NonTerminalIndex>(symbol)) {
            return std::get<NonTerminalIndex>(symbol).m_Index;
        }

        return std::get<TerminalIndex>(symbol).m_Index;
    }

    std::string const &GetSymbolName(
            std::vector<NonTerminal> const &nonTerminals,
            std::vector<Terminal> const &terminals, Symbol const &symbol
    ) {
        if (std::holds_alternative<std::nullptr_t>(symbol)) {
            return c_Epsilon;
        }

        if (std::holds_alternative<NonTerminalIndex>(symbol)) {
            auto const index{std::get<NonTerminalIndex>(symbol).m_Index};
            return nonTerminals.at(index).m_Name;
        }

        auto const index{std::get<TerminalIndex>(symbol).m_Index};
        return terminals.at(index).m_Token;
    }

    bool Item::operator==(Item const &other) const {
        return m_Production == other.m_Production &&
               m_Position == other.m_Position &&
               m_LookAhead == other.m_LookAhead;
    }

    Symbol Item::GetSymbolAtPosition(std::size_t offset) const {
        if (m_Position + offset >= m_Production->m_Symbols.size()) {
            return nullptr;
        }

        return m_Production->m_Symbols.at(m_Position + offset);
    }

    std::unordered_set<Symbol, SymbolHash>
    Item::GetSymbolsAfterPosition() const {
        std::unordered_set<Symbol, SymbolHash> symbols;
        if (static_cast<std::size_t>(m_Position) + 1 >=
            m_Production->m_Symbols.size()) {
            return symbols;
        }

        std::copy(
                m_Production->m_Symbols.cbegin() + m_Position + 1,
                m_Production->m_Symbols.cend(),
                std::inserter(symbols, symbols.end())
        );

        return symbols;
    }

    bool Item::HasNextSymbol() const {
        return static_cast<std::size_t>(m_Position) <
               m_Production->m_Symbols.size();
    }

    std::size_t ItemHash::operator()(Item const &item) const {
        return std::hash<Production const *>{}(item.m_Production) ^
               std::hash<int>{}(item.m_Position);
    }

    std::size_t ItemSetHash::operator()(ItemSet const &itemSet) const {
        return std::accumulate(
                itemSet.cbegin(), itemSet.cend(), std::size_t{},
                [](std::size_t hash, Item const &item) {
                    return hash ^ ItemHash {}(item);
                }
        );
    }
}// namespace jcc::parser_gen
