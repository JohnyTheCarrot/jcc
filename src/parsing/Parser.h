//
// Created by johny on 7/20/23.
//

#ifndef JCC_PARSER_H
#define JCC_PARSER_H


#include "../tokenizer.h"
#include "ASTNode.h"
#include "ParserRuleBuilder.h"

class Parser {
public:
    Parser(TokenList &&tokenList, const std::string &fileName, std::istream &inputStream);

    void Parse();

    ParserRuleBuilder Expect(TokenType tokenType);

    std::optional<Token> PeekNextToken();

    void AdvanceCursor();

    std::optional<Token> ConsumeIfTokenIs(TokenType tokenType);

    _Noreturn void Error(const Span &span, const std::string &message);

private:
    std::string fileName;
    std::istream &inputStream;
    TokenList tokens;
    int cursor{-1};
};


#endif //JCC_PARSER_H
