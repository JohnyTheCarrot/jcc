//
// Created by johny on 7/20/23.
//

#ifndef JCC_PARSER_H
#define JCC_PARSER_H


#include "../tokenizer.h"

class Parser {
public:
    Parser(TokenList &&tokenList, std::string fileName, std::istream &inputStream);

    void Parse();

    bool operator!() const;

    explicit operator bool() const;

    const Token& PeekNextToken();

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

    const Token &ExpectToken(TokenType tokenType, Span &spanToAddTo);

	const Token &ExpectToken(TokenType tokenType);

    bool AdvanceIfTokenIs(TokenType tokenType, Token &token);

    bool AdvanceIfTokenIs(TokenType tokenType);

    [[noreturn]]
    void Error(const Span &span, const std::string &message) const;

    [[noreturn]]
    void Error(const std::string &message) const;

private:
    std::string fileName;
    std::istream &inputStream;
    TokenList tokens;
    int cursor{-1};
};


#endif //JCC_PARSER_H
