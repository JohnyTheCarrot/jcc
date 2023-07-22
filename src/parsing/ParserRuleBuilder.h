//
// Created by johny on 7/21/23.
//

#ifndef JCC_PARSERRULEBUILDER_H
#define JCC_PARSERRULEBUILDER_H


#include <vector>
#include <memory>
#include "../tokenizer.h"
#include "ASTNode.h"

class Parser;

class ParserRuleBuilder;

using TokenMatch = std::optional<Token>;

struct ParserRule {
    ParserRule() = default;

    ParserRule(ParserRule &&) = default;

    virtual ~ParserRule() = default;

    virtual void Match(Parser &parser) = 0;

    TokenMatch match;
    std::optional<Span> foundSpan;
};

struct ParserRuleKnownResult : public ParserRule {
    explicit ParserRuleKnownResult(const TokenMatch &result, const Span &foundSpan) : knownResult{result}, foundSpan{foundSpan} {};

    void Match(Parser &parser) override;

    TokenMatch knownResult;
    std::optional<Span> foundSpan;
};

struct ParserRuleExact : public ParserRule {
    explicit ParserRuleExact(TokenType type) : tokenType{type} {};

    void Match(Parser &parser) override;

    TokenType tokenType;
};

template <class Node>
struct ParserRuleNodeTest : public ParserRule {
    ParserRuleNodeTest() {
        static_assert(std::is_base_of<ASTNode, Node>::value, "Node must be derived from ASTNode.");
    }

    void Match(Parser &parser) override;
};

struct ParserRuleFollowedBy : public ParserRule {
    ParserRuleFollowedBy(std::unique_ptr<ParserRule> &&left, std::unique_ptr<ParserRule> &&right)
    : left{std::move(left)}, right{std::move(right)} {}

    void Match(Parser &parser) override;

    std::unique_ptr<ParserRule> left;

    std::unique_ptr<ParserRule> right;
};

struct ParserRuleOr : public ParserRule {
    ParserRuleOr(std::unique_ptr<ParserRule> &&left, std::unique_ptr<ParserRule> &&right)
            : left{std::move(left)}, right{std::move(right)} {}

    void Match(Parser &parser) override;

    std::unique_ptr<ParserRule> left;

    std::unique_ptr<ParserRule> right;
};

class ParserRuleBuilder {
public:
    ParserRuleBuilder(Parser &parser, TokenType expectedTokenType) : parser{parser} {
        this->rootRule = std::make_unique<ParserRuleExact>(expectedTokenType);
    };

    ParserRuleBuilder &Or(std::unique_ptr<ParserRule> &&rule);

    ParserRuleBuilder &Or(TokenType expected);

    ParserRuleBuilder &Or(ParserRuleBuilder &&rule);

    template<class Node>
    ParserRuleBuilder &Or();

    ParserRuleBuilder &FollowedBy(std::unique_ptr<ParserRule> &&rule);

    ParserRuleBuilder &FollowedBy(TokenType expected);

    ParserRuleBuilder &FollowedBy(ParserRuleBuilder &&rule);

    template<class Node>
    ParserRuleBuilder &FollowedBy();

    static std::unique_ptr<ParserRule> &&GetRule(ParserRuleBuilder &&builder);

    ParserRuleBuilder &Match(bool &didMatch, Span &foundSpan);

    ParserRuleBuilder &Match(TokenMatch &matchedToken, Span &foundSpan);
private:
    Parser &parser;

    std::unique_ptr<ParserRule> rootRule;
};


#endif //JCC_PARSERRULEBUILDER_H
