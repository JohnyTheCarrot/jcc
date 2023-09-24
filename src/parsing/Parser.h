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

    template<class TNodeBuilder, class TOutNode, class ...TRest>
    static ParserRuleBuilder<TNodeBuilder, TOutNode, TRest...> Expect(const TNodeBuilder &nodeBuilder) {
        return ParserRuleBuilder<TNodeBuilder, TOutNode, TRest...>{nodeBuilder};
    }

    [[nodiscard]]
    static ParserRuleToken Expect(TokenType expected) {
        return ParserRuleToken{expected};
    }

    template<class TOutNode>
    static ParserRuleNode<TOutNode> Expect(std::optional<TOutNode> &out) {
        return ParserRuleNode<TOutNode>{out};
    }

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
