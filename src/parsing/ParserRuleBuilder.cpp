//
// Created by johny on 7/21/23.
//

#include "ParserRuleBuilder.h"
#include "../reporting.h"
#include "fmt/format.h"
#include "Parser.h"
#include "../../libs/magic_enum/magic_enum.hpp"

void ParserRuleKnownResult::Match(Parser &parser) {
    this->match = std::move(this->knownResult);
}

void ParserRuleExact::Match(Parser &parser) {
    std::optional<Token> token{parser.PeekNextToken()};

    if (!token.has_value())
    {
        Span span{};

        std::optional<Token> lastToken{parser.GetLastToken()};
        if (lastToken.has_value())
            span = lastToken->span;

        this->match = ParseResultError(span, fmt::format("Expected token of type {}", magic_enum::enum_name(this->tokenType)));
        return;
    }

    this->foundSpan = token->span;

    if (token->type == this->tokenType)
    {
        parser.AdvanceCursor();
        this->match = token.value();
        return;
    }

    this->match = ParseResultError(token->span, fmt::format("Expected token of type {}", magic_enum::enum_name(this->tokenType)));
}

void ParserRuleFollowedBy::Match(Parser &parser) {
    this->left->Match(parser);

    // left node didn't value, so the FollowedBy check fails by definition
    if (!this->left->match) {
        this->match.value = ParseResultError(this->left->foundSpan, "FollowedBy failed");

        this->foundSpan = this->left->foundSpan;
        return;
    }

    this->right->Match(parser);
    this->match = std::move(this->right->match);
    this->foundSpan = this->right->foundSpan;
}

void ParserRuleOr::Match(Parser &parser) {
    this->left->Match(parser);

    this->foundSpan = this->left->foundSpan;

    if (this->left->match) {
        this->match = std::move(this->left->match);
        return;
    }

    this->right->Match(parser);
    this->match = std::move(this->right->match);
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

std::unique_ptr<ParserRule> &&ParserRuleBuilder::GetRule(ParserRuleBuilder &&builder) {
    return std::move(builder.rootRule);
}

ParserRuleBuilder &ParserRuleBuilder::Match(TokenMatch &matchedToken, Span &foundSpan) {
    this->rootRule->Match(this->parser);

    matchedToken = std::move(this->rootRule->match.Clone());
    if (!this->rootRule->match) {
        foundSpan = std::get<ParseResultError>(this->rootRule->match.value).span;
        this->parser.SetCursor(this->startCursor);
    }
    else
        foundSpan = this->rootRule->foundSpan;

    this->rootRule = std::move(std::make_unique<ParserRuleKnownResult>(std::move(matchedToken), foundSpan));

    return *this;
}

ParserRuleBuilder &ParserRuleBuilder::Match() {
    Span foundSpan;
    TokenMatch matchedToken;

    this->Match(matchedToken, foundSpan);

    return *this;
}

ParserRuleBuilder &ParserRuleBuilder::Match(bool &didMatch, Span &foundSpan) {
    TokenMatch matchedToken;

    this->Match(matchedToken, foundSpan);

    didMatch = matchedToken;

    return *this;
}

ParserRuleBuilder::ParserRuleBuilder(Parser &parser, std::unique_ptr<ParserRule> &&rule) : startCursor{parser.GetCursor()}, parser{parser} {
    this->rootRule = std::move(rule);
}

ParserRuleBuilder::ParserRuleBuilder(Parser &parser, TokenType expectedTokenType) : parser{parser} {
    this->rootRule = std::make_unique<ParserRuleExact>(expectedTokenType);
};

TokenMatch &TokenMatch::operator=(MatchInner &&optMatch) {
    if (std::holds_alternative<Token>(optMatch)) {
        this->valueType = ValueType::Token;
    } else if (std::holds_alternative<std::unique_ptr<ASTNode>>(optMatch)) {
        this->valueType = ValueType::Node;
    } else if (std::holds_alternative<ParseResultError>(optMatch)) {
        this->valueType = ValueType::Error;
    } else {
        assert(false);
    }

    this->value = std::move(optMatch);

    return *this;
}
