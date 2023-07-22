//
// Created by johny on 7/21/23.
//

#include "ParserRuleBuilder.h"
#include "../reporting.h"
#include "fmt/format.h"
#include "Parser.h"

void ParserRuleKnownResult::Match(Parser &parser) {
    this->match = this->knownResult;
}

void ParserRuleExact::Match(Parser &parser) {
    std::optional<Token> token{parser.PeekNextToken()};

    if (!token.has_value())
    {
        this->match = std::nullopt;

        return;
    }

    if (token->type == this->tokenType)
    {
        parser.AdvanceCursor();
        this->match = token;
    }

    if (!this->foundSpan.has_value())
        this->foundSpan = token->span;
}

template<class Node>
void ParserRuleNodeTest<Node>::Match(Parser &parser) {
    std::unique_ptr<ASTNode> astNode{ std::make_unique<Node>() };

    astNode->Parse(parser);
}

void ParserRuleFollowedBy::Match(Parser &parser) {
    this->left->Match(parser);

    // left node didn't match, so the FollowedBy check fails by definition
    if (!this->left->match.has_value()) {
        this->match = std::nullopt;
        this->foundSpan = this->left->foundSpan;

        return;
    }

    this->right->Match(parser);
    this->match = this->right->match;
    this->foundSpan = this->right->foundSpan;
}

void ParserRuleOr::Match(Parser &parser) {
    this->left->Match(parser);

    this->foundSpan = this->left->foundSpan;

    if (this->left->match.has_value()){
        this->match = this->left->match;
        return;
    }

    this->right->Match(parser);
    this->match = this->right->match;
    this->foundSpan = this->right->foundSpan;
}

ParserRuleBuilder &ParserRuleBuilder::Or(std::unique_ptr<ParserRule> &&rule) {
    std::unique_ptr<ParserRule> &&currentRule{ std::move(this->rootRule) };

    std::unique_ptr<ParserRuleOr> ruleOr{ std::make_unique<ParserRuleOr>(std::move(currentRule), std::move(rule)) };

    this->rootRule = std::move(ruleOr);

    return *this;
}

ParserRuleBuilder &ParserRuleBuilder::Or(TokenType expected) {
    std::unique_ptr<ParserRule> &&otherRule{ std::make_unique<ParserRuleExact>(expected) };

    return this->Or(std::move(otherRule));
}

ParserRuleBuilder &ParserRuleBuilder::Or(ParserRuleBuilder &&rule) {
    std::unique_ptr<ParserRule> &&otherRule{ GetRule(std::move(rule)) };

    return this->Or(std::move(otherRule));
}

template<class Node>
ParserRuleBuilder &ParserRuleBuilder::Or() {
    std::unique_ptr<ParserRule> &&otherRule{ std::make_unique<ParserRuleNodeTest<Node>>() };

    return this->Or(std::move(otherRule));
}

ParserRuleBuilder &ParserRuleBuilder::FollowedBy(std::unique_ptr<ParserRule> &&rule) {
    std::unique_ptr<ParserRule> &&currentRule{ std::move(this->rootRule) };

    std::unique_ptr<ParserRuleFollowedBy> ruleFollowedBy{ std::make_unique<ParserRuleFollowedBy>(std::move(currentRule), std::move(rule)) };

    this->rootRule = std::move(ruleFollowedBy);

    return *this;
}

ParserRuleBuilder &ParserRuleBuilder::FollowedBy(TokenType expected) {
    std::unique_ptr<ParserRule> &&otherRule{ std::make_unique<ParserRuleExact>(expected) };

    return this->FollowedBy(std::move(otherRule));
}

ParserRuleBuilder &ParserRuleBuilder::FollowedBy(ParserRuleBuilder &&rule) {
    std::unique_ptr<ParserRule> &&otherRule{ GetRule(std::move(rule)) };

    return this->FollowedBy(std::move(otherRule));
}

template<class Node>
ParserRuleBuilder &ParserRuleBuilder::FollowedBy() {
    std::unique_ptr<ParserRule> &&otherRule{ std::make_unique<ParserRuleNodeTest<Node>>() };

    return this->FollowedBy(std::move(otherRule));
}

std::unique_ptr<ParserRule> &&ParserRuleBuilder::GetRule(ParserRuleBuilder &&builder) {
    return std::move(builder.rootRule);
}

ParserRuleBuilder &ParserRuleBuilder::Match(bool &didMatch, Span &foundSpan) {
    this->rootRule->Match(this->parser);

    didMatch = this->rootRule->match.has_value();
    if (this->rootRule->foundSpan.has_value())
        foundSpan = this->rootRule->foundSpan.value();

    return *this;
}

ParserRuleBuilder &ParserRuleBuilder::Match(TokenMatch &matchedToken, Span &foundSpan) {
    this->rootRule->Match(this->parser);

    matchedToken = this->rootRule->match;
    if (this->rootRule->foundSpan.has_value())
        foundSpan = this->rootRule->foundSpan.value();

    this->rootRule = std::make_unique<ParserRuleKnownResult>(matchedToken, foundSpan);

    return *this;
}
