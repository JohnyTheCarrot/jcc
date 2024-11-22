#ifndef GRAMMAR_PARSER_H
#define GRAMMAR_PARSER_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "grammar_parser.hpp"
#include "production.hpp"

namespace jcc::parser_gen {
    using Grammar = std::vector<Production>;

    using Terminals    = std::vector<Terminal>;
    using NonTerminals = std::vector<NonTerminal>;

    struct LanguageInfo final {
        Terminals    m_Terminals;
        NonTerminals m_NonTerminals;
        Grammar      m_Grammar;
    };

    class GrammarParserController final {
        using TokenMap = std::unordered_map<std::string, Terminal>;

        TokenMap     m_TokenMap;
        Terminals    m_Terminals{Terminal::c_Eof};
        NonTerminals m_NonTerminals{NonTerminal::c_SPrime};
        Grammar      m_Grammar;

    public:
        TerminalIndex AddTerminal(std::string token, std::string value);

        [[nodiscard]]
        std::optional<TerminalIndex> GetTerminalIndex(std::string const &token
        ) const;

        NonTerminalIndex AddNonTerminal(std::string name);

        [[nodiscard]]
        std::optional<NonTerminalIndex>
        GetNonTerminalIndex(std::string const &name) const;

        [[nodiscard]]
        Symbol GetSymbol(std::string const &name) const;

        Production &AddProduction(NonTerminalIndex nonTerminalIndex);

        [[nodiscard]]
        LanguageInfo GetLanguageInfo();
    };

    class GrammarParserCommand {
    public:
        virtual ~GrammarParserCommand() = default;

        virtual void
        Execute(GrammarParserController &controller,
                std::string_view         rest) const = 0;
    };

    class TokenCommand final : public GrammarParserCommand {
    public:
        TokenCommand();

        void
        Execute(GrammarParserController &controller,
                std::string_view         rest) const override;
    };

    namespace utils {
        [[nodiscard]]
        std::string_view::const_iterator FirstSpace(std::string_view line);

        [[nodiscard]]
        std::string::const_iterator FirstSpace(std::string const &line);

        [[nodiscard]]
        std::string_view::const_iterator FirstNonSpace(std::string_view line);

        [[nodiscard]]
        std::string::const_iterator FirstNonSpace(std::string const &line);
    }// namespace utils

    class GrammarParser final {
        using CommandMap = std::unordered_map<
                std::string, std::unique_ptr<GrammarParserCommand>>;
        CommandMap const m_Commands{[] {
            CommandMap map;

            map["token"] = std::make_unique<TokenCommand>();

            return map;
        }()};

        GrammarParserController m_Controller;

        std::optional<NonTerminalIndex> m_CurrentNonTerminal;

        void ParseCommand(std::string_view line);

        void ParseNonTerminalDeclaration(std::string_view line);

        void ParseProduction(std::string_view line);

    public:
        GrammarParser();

        [[nodiscard]]
        LanguageInfo Parse(std::string const &grammarPath);
    };
}// namespace jcc::parser_gen

#endif//GRAMMAR_PARSER_H
