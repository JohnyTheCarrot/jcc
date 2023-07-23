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

struct TokenMatch {
    using MatchInner = std::variant<Token, std::unique_ptr<ASTNode>, ParseResultError>;

    enum class ValueType {
        Error,
        Token,
        Node,
    };

    TokenMatch() : valueType{ValueType::Error}, value{nullptr} {}

    TokenMatch(ValueType type, MatchInner &&token) : valueType{type}, value{std::move(token)} {};

    TokenMatch(TokenMatch &) = delete;

    TokenMatch &operator=(TokenMatch &) = delete;

    TokenMatch &operator=(MatchInner &&optMatch);

    operator bool() const {
        return !std::holds_alternative<ParseResultError>(this->value);
    }

    TokenMatch(TokenMatch &&other)  noexcept : valueType{other.valueType}, value{std::move(other.value)} {};

    TokenMatch &operator=(TokenMatch &&other) noexcept {
        this->valueType = other.valueType;
        this->value = std::move(other.value);

        return *this;
    }

    ValueType valueType{};
    MatchInner value{nullptr};

    TokenMatch Clone() {
        TokenMatch clone{};
        clone.valueType = this->valueType;

        if (std::holds_alternative<Token>(this->value)) {
            clone.value = std::get<Token>(this->value);
        } else if (std::holds_alternative<std::unique_ptr<ASTNode>>(this->value)) {
            clone.value = std::get<std::unique_ptr<ASTNode>>(this->value)->Clone();
        } else if (std::holds_alternative<ParseResultError>(this->value)) {
            clone.value = std::get<ParseResultError>(this->value);
        }

        return clone;
    }
};

struct ParserRule {
    ParserRule() = default;

    virtual ~ParserRule() = default;

    virtual void Match(Parser &parser) = 0;

    TokenMatch match{};
    Span foundSpan;
};

struct ParserRuleKnownResult : public ParserRule {
    ParserRuleKnownResult(TokenMatch &&result, const Span &foundSpan) : knownResult{std::move(result)}, foundSpan{foundSpan} {};

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

    void Match(Parser &parser) override {
        std::unique_ptr<ASTNode> astNode{ std::make_unique<Node>() };

        ParseResult parseResult{astNode->Parse(parser)};

        if (parseResult.has_value()) {
            this->foundSpan = parseResult.value().span;
            this->match = std::move(parseResult.value());
        }
        else {
            // todo: figure the span
            this->match = std::move(astNode);
        }
    }
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
    ParserRuleBuilder(Parser &parser, std::unique_ptr<ParserRule> &&rule) : parser{parser} {
        this->rootRule = std::move(rule);
    };

    ParserRuleBuilder(Parser &parser, TokenType expectedTokenType) : parser{parser} {
        this->rootRule = std::make_unique<ParserRuleExact>(expectedTokenType);
    };

    ParserRuleBuilder &Or(std::unique_ptr<ParserRule> &&rule);

    ParserRuleBuilder &Or(TokenType expected);

    ParserRuleBuilder &Or(ParserRuleBuilder &&rule);

    template<class Node>
    ParserRuleBuilder &Or() {
        std::unique_ptr<ParserRule> &&otherRule{ std::make_unique<ParserRuleNodeTest<Node>>() };

        return this->Or(std::move(otherRule));
    }

    ParserRuleBuilder &FollowedBy(std::unique_ptr<ParserRule> &&rule);

    ParserRuleBuilder &FollowedBy(TokenType expected);

    ParserRuleBuilder &FollowedBy(ParserRuleBuilder &&rule);

    template<class Node>
    ParserRuleBuilder &FollowedBy() {
        std::unique_ptr<ParserRule> &&otherRule{ std::make_unique<ParserRuleNodeTest<Node>>() };

        return this->FollowedBy(std::move(otherRule));
    }

    static std::unique_ptr<ParserRule> &&GetRule(ParserRuleBuilder &&builder);

    ParserRuleBuilder &Match(bool &didMatch, Span &foundSpan);

    ParserRuleBuilder &Match(TokenMatch &matchedToken, Span &foundSpan);
private:
    Parser &parser;

    std::unique_ptr<ParserRule> rootRule;
};


#endif //JCC_PARSERRULEBUILDER_H
