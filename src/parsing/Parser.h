//
// Created by johny on 7/20/23.
//

#ifndef JCC_PARSER_H
#define JCC_PARSER_H


#include "../tokenizer.h"
#include "ParserRuleBuilder.h"

class Parser {
public:
    Parser(TokenList &&tokenList, const std::string &fileName, std::istream &inputStream);

    void Parse();

    template<class Node>
    ParserRuleBuilder Expect() {
        static_assert(std::is_base_of<ASTNode, Node>::value, "Node must be derived from ASTNode.");

        std::unique_ptr<ParserRule> rule{std::make_unique<ParserRuleNodeTest<Node>>()};

        return ParserRuleBuilder{*this, std::move(rule)};
    }

    template<class TNodeBuilder, class TOutNode, class ...TRest>
    static ParserRuleNewBuilder<TNodeBuilder, TOutNode, TRest...> ExpectNew(const TNodeBuilder &nodeBuilder) {
        return ParserRuleNewBuilder<TNodeBuilder, TOutNode, TRest...>{nodeBuilder};
    }

    [[nodiscard]]
    static ParserRuleNewToken ExpectNew(TokenType expected) {
        return ParserRuleNewToken{expected};
    }

    template<class TNodeOut>
    static ParserRuleNewNode<TNodeOut> ExpectNew(std::optional<TNodeOut> &out) {
        return ParserRuleNewNode<TNodeOut>{out};
    }

    ParserRuleBuilder Expect(ParserRuleBuilder &&ruleBuilder);

    ParserRuleBuilder Expect(TokenType tokenType);

    std::optional<Token> PeekNextToken();

    [[nodiscard]]
    std::optional<Token> GetToken(size_t index) const {
        if (index >= this->tokens.size())
            return std::nullopt;

        return *this->tokens[index];
    };

    [[nodiscard]]
    std::optional<Token> GetLastToken() const {
        if (this->tokens.empty())
            return std::nullopt;

        return *this->tokens[this->tokens.size() - 1];
    };

    void AdvanceCursor();

    [[nodiscard]]
    int GetCursor() const {
        return this->cursor;
    }

    void SetCursor(int newCursor) {
        this->cursor = newCursor;
    }

    [[nodiscard]]
    std::optional<Token> ConsumeIfTokenIs(TokenType tokenType);

    [[noreturn]]
    void Error(const Span &span, const std::string &message);

private:
    std::string fileName;
    std::istream &inputStream;
    TokenList tokens;
    int cursor{-1};
};


#endif //JCC_PARSER_H
