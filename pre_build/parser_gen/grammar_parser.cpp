#include "grammar_parser.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>

namespace jcc::parser_gen {
    TerminalIndex
    GrammarParserController::AddTerminal(std::string token, std::string value) {
        Terminal terminal{std::move(value)};
        if (std::ranges::find(
                    m_Terminals.cbegin(), m_Terminals.cend(), terminal
            ) != m_Terminals.cend()) {
            throw std::runtime_error{
                    std::format("Token {} is already mapped", token)
            };
        }
        m_Terminals.emplace_back(terminal);
        m_TokenMap[std::move(token)] = std::move(terminal);

        return {m_Terminals.size() - 1};
    }

    std::optional<TerminalIndex>
    GrammarParserController::GetTerminalIndex(std::string const &token) const {
        if (auto const it{std::find_if(
                    m_TokenMap.cbegin(), m_TokenMap.cend(),
                    [&token](auto const &pair) { return pair.first == token; }
            )};
            it != m_TokenMap.cend()) {
            auto const terminalIndex{std::find_if(
                    m_Terminals.cbegin(), m_Terminals.cend(),
                    [&it](auto const &terminal) {
                        return it->second == terminal;
                    }
            )};
            return TerminalIndex{static_cast<std::size_t>(
                    std::distance(m_Terminals.cbegin(), terminalIndex)
            )};
        }

        return std::nullopt;
    }

    NonTerminalIndex GrammarParserController::AddNonTerminal(std::string name) {
        NonTerminal nonTerminal{std::move(name)};
        if (auto const it{std::find(
                    m_NonTerminals.cbegin(), m_NonTerminals.cend(), nonTerminal
            )};
            it != m_NonTerminals.cend()) {
            return {static_cast<std::size_t>(
                    std::distance(m_NonTerminals.cbegin(), it)
            )};
        }

        m_NonTerminals.emplace_back(std::move(nonTerminal));

        return {m_NonTerminals.size() - 1};
    }

    std::optional<NonTerminalIndex>
    GrammarParserController::GetNonTerminalIndex(std::string const &name
    ) const {
        if (auto const it{std::ranges::find(m_NonTerminals, NonTerminal{name})};
            it != m_NonTerminals.end()) {
            return NonTerminalIndex{static_cast<std::size_t>(
                    std::distance(m_NonTerminals.cbegin(), it)
            )};
        }

        return std::nullopt;
    }

    Symbol GrammarParserController::GetSymbol(std::string const &name) const {
        if (auto const terminalIndex{GetTerminalIndex(name)};
            terminalIndex.has_value()) {
            return terminalIndex.value();
        }

        if (auto const nonTerminalIndex{GetNonTerminalIndex(name)};
            nonTerminalIndex.has_value()) {
            return nonTerminalIndex.value();
        }

        throw std::runtime_error{std::format("Symbol {} not found", name)};
    }

    Production &
    GrammarParserController::AddProduction(NonTerminalIndex nonTerminalIndex) {
        return m_Grammar.emplace_back(Production{
                nonTerminalIndex, {}, static_cast<int>(m_Grammar.size())
        });
    }

    LanguageInfo GrammarParserController::GetLanguageInfo() {
        std::vector<Terminal> terminals;
        std::ranges::move(m_Terminals, std::back_inserter(terminals));

        std::vector<NonTerminal> nonTerminals;
        std::ranges::move(m_NonTerminals, std::back_inserter(nonTerminals));

        return LanguageInfo{
                .m_Terminals    = std::move(terminals),
                .m_NonTerminals = std::move(nonTerminals),
                .m_Grammar      = std::move(m_Grammar)
        };
    }

    TokenCommand::TokenCommand() = default;

    void TokenCommand::Execute(
            GrammarParserController &controller, std::string_view rest
    ) const {
        auto const firstSpace{utils::FirstSpace(rest)};

        std::string name{rest.cbegin(), firstSpace};

        auto const valueStart{
                utils::FirstNonSpace(std::string_view{firstSpace, rest.cend()})
        };

        std::string value{valueStart, rest.cend()};

        controller.AddTerminal(std::move(name), std::move(value));
    }

    std::string_view::const_iterator utils::FirstSpace(std::string_view line) {
        return std::ranges::find_if(line, [](char c) {
            return std::isspace(c);
        });
    }

    std::string::const_iterator utils::FirstSpace(std::string const &line) {
        return std::ranges::find_if(line, [](char c) {
            return std::isspace(c);
        });
    }

    std::string_view::const_iterator utils::FirstNonSpace(std::string_view line
    ) {
        return std::ranges::find_if(line, [](char c) {
            return !std::isspace(c);
        });
    }

    std::string::const_iterator utils::FirstNonSpace(std::string const &line) {
        return std::ranges::find_if(line, [](char c) {
            return !std::isspace(c);
        });
    }

    void GrammarParser::ParseCommand(std::string_view line) {
        auto const firstSpace{utils::FirstSpace(line)};

        std::string_view const command{line.cbegin(), firstSpace};

        if (auto const it{m_Commands.find(std::string{command})};
            it != m_Commands.end()) {
            auto const commandArgsStart{utils::FirstNonSpace(
                    std::string_view{firstSpace, line.cend()}
            )};

            it->second->Execute(
                    m_Controller,
                    std::string_view{commandArgsStart, line.cend()}
            );
        } else {
            throw std::runtime_error{"Invalid command"};
        }
    }

    void GrammarParser::ParseNonTerminalDeclaration(std::string_view line) {
        if (m_CurrentNonTerminal.has_value()) {
            throw std::runtime_error{"Please finish the current non-terminal"};
        }

        auto const firstSpace{utils::FirstSpace(line)};

        std::string name{line.cbegin(), firstSpace};
        if (auto const index{m_Controller.GetNonTerminalIndex(name)};
            index != std::nullopt) {
            m_CurrentNonTerminal = index;
            return;
        }

        m_CurrentNonTerminal = m_Controller.AddNonTerminal(std::move(name));
    }

    void GrammarParser::ParseProduction(std::string_view line) {
        if (!m_CurrentNonTerminal.has_value()) {
            throw std::runtime_error{"No non-terminal declared"};
        }

        auto &production{m_Controller.AddProduction(m_CurrentNonTerminal.value()
        )};

        auto firstNonSpace{utils::FirstNonSpace(line)};

        while (firstNonSpace != line.cend()) {
            auto const        nextSpace{utils::FirstSpace(
                    std::string_view{firstNonSpace, line.cend()}
            )};
            std::string const token{firstNonSpace, nextSpace};

            auto const symbol{m_Controller.GetSymbol(token)};
            production.m_Symbols.push_back(symbol);

            firstNonSpace = utils::FirstNonSpace(
                    std::string_view{nextSpace, line.cend()}
            );
        }
    }

    GrammarParser::GrammarParser() = default;

    LanguageInfo GrammarParser::Parse(std::string const &grammarPath) {
        std::ifstream file{grammarPath};
        if (!file.is_open()) {
            throw std::runtime_error{"Failed to open file"};
        }

        // for every line in the file
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            // skip whitespace
            auto const firstNonSpace{utils::FirstNonSpace(line)};
            if (firstNonSpace == line.cend()) {
                continue;
            }
            std::string_view const rest{std::next(firstNonSpace), line.cend()};
            switch (*firstNonSpace) {
                case '%':
                    ParseCommand(rest);
                    break;
                case ':':
                case '|':
                    ParseProduction(rest);
                    break;
                case ';':
                    if (!m_CurrentNonTerminal.has_value()) {
                        throw std::runtime_error{"No non-terminal declared"};
                    }
                    m_CurrentNonTerminal.reset();
                    break;
                case '#':
                    break;
                default:
                    ParseNonTerminalDeclaration(
                            std::string_view{firstNonSpace, line.cend()}
                    );
                    break;
            }
        }

        return m_Controller.GetLanguageInfo();
    }
}// namespace jcc::parser_gen
