//
// Created by johny on 7/21/23.
//

#ifndef JCC_PARSERRULEBUILDER_H
#define JCC_PARSERRULEBUILDER_H


#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include "../tokenizer.h"
#include "ASTNode.h"
#include "../../libs/magic_enum/magic_enum.hpp"

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

    TokenMatch(TokenMatch &&other) noexcept: valueType{other.valueType}, value{std::move(other.value)} {};

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
    ParserRuleKnownResult(TokenMatch &&result, const Span &foundSpan) : knownResult{std::move(result)},
                                                                        foundSpan{foundSpan} {};

    void Match(Parser &parser) override;

    TokenMatch knownResult;
    std::optional<Span> foundSpan;
};

struct ParserRuleExact : public ParserRule {
    explicit ParserRuleExact(TokenType type) : tokenType{type} {};

    void Match(Parser &parser) override;

    TokenType tokenType;
};

template<class Node>
struct ParserRuleNodeTest : public ParserRule {
    ParserRuleNodeTest() {
        static_assert(std::is_base_of<ASTNode, Node>::value, "Node must be derived from ASTNode.");
    }

    void Match(Parser &parser) override {
        std::unique_ptr<ASTNode> astNode{std::make_unique<Node>()};

        ParseResult parseResult{astNode->Parse(parser)};

        if (parseResult.has_value()) {
            this->foundSpan = parseResult.value().span;
            this->match = std::move(parseResult.value());
        } else {
            // todo: figure out the span
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
    ParserRuleBuilder(Parser &parser, std::unique_ptr<ParserRule> &&rule);

    ParserRuleBuilder(Parser &parser, TokenType expectedTokenType);

    ParserRuleBuilder(ParserRuleBuilder &&rvalueOther) noexcept: parser{rvalueOther.parser},
                                                                 rootRule{std::move(rvalueOther.rootRule)} {};

    ParserRuleBuilder &Or(std::unique_ptr<ParserRule> &&rule);

    ParserRuleBuilder &Or(TokenType expected);

    ParserRuleBuilder &Or(ParserRuleBuilder &&rule);

    template<class Node>
    ParserRuleBuilder &Or() {
        std::unique_ptr<ParserRule> &&otherRule{std::make_unique<ParserRuleNodeTest<Node>>()};

        return this->Or(std::move(otherRule));
    }

    ParserRuleBuilder &FollowedBy(std::unique_ptr<ParserRule> &&rule);

    ParserRuleBuilder &FollowedBy(TokenType expected);

    ParserRuleBuilder &FollowedBy(ParserRuleBuilder &&rule);

    template<class Node>
    ParserRuleBuilder &FollowedBy() {
        std::unique_ptr<ParserRule> &&otherRule{std::make_unique<ParserRuleNodeTest<Node>>()};

        return this->FollowedBy(std::move(otherRule));
    }

    static std::unique_ptr<ParserRule> &&GetRule(ParserRuleBuilder &&builder);

    ParserRuleBuilder &Match();

    ParserRuleBuilder &Match(bool &didMatch, Span &foundSpan);

    ParserRuleBuilder &Match(TokenMatch &matchedToken, Span &foundSpan);

private:
    Parser &parser;

    std::unique_ptr<ParserRule> rootRule;
};

// new

[[noreturn]]
void Error(Parser &parser, const std::string &message);

template<class TRule, class TOutNode, class ...TRuleOutNode>
struct NodeBuilder {
    using BuilderFunction = std::function<TOutNode(TRuleOutNode...)>;
    using OutNode = TOutNode;

    NodeBuilder(
            TRule &&rule,
            BuilderFunction builder
    ) : rule{std::move(rule)}
      , builder{builder} {}

    TOutNode Match(Parser &parser) const {
        std::optional<typename TRule::OutNode> parseResult{this->rule.Match(parser)};

        if (!parseResult.has_value()) {
            std::stringstream errorMessage;
            errorMessage << "Expected\n" << this->rule << ".";

            Error(parser, errorMessage.str());
        }

        return std::apply(this->builder, std::tuple(std::move(parseResult.value())));
    }

    friend std::ostream &operator<<(std::ostream &stream, const NodeBuilder &ruleToOutput) {
        stream << ruleToOutput.rule;

        return stream;
    }

    TRule rule;
    BuilderFunction builder;
};

class ParserRuleNewToken {
public:
    using OutNode = Token;

    explicit ParserRuleNewToken(TokenType tokenType) : toMatch{tokenType} {}

    ParserRuleNewToken &Or(TokenType tokenType);

    template<class TOutNode>
    NodeBuilder<ParserRuleNewToken, TOutNode, Token> Builder(const std::function<TOutNode(Token)> &builder) {
        return {
                std::move(*this),
                builder
        };
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleNewToken &ruleToOutput) {
        if (!ruleToOutput.alternatives.empty())
            stream << "(\n\t  ";

        stream << magic_enum::enum_name(ruleToOutput.toMatch);

        for (const TokenType &item: ruleToOutput.alternatives) {
            stream << "\n\t| " << magic_enum::enum_name(item);
        }

        if (!ruleToOutput.alternatives.empty())
            stream << "\n)";

        return stream;
    }

    std::optional<OutNode> Match(Parser &parser) const;

private:
    TokenType toMatch;
    std::vector<TokenType> alternatives{};
};

template<class TRule, class ...TBuilderParams>
class ParserRuleNewFollowedByIgnore;

template<class TRule, class TFollowedBy>
class ParserRuleNewFollowedBy {
public:
    using OutNode = std::tuple<typename TRule::OutNode, typename TFollowedBy::OutNode>;

    ParserRuleNewFollowedBy(const TRule &rule, const TFollowedBy &followedBy) : rule{rule}, followedBy{followedBy} {}

    template<class TOutNode>
    NodeBuilder<ParserRuleNewFollowedBy, TOutNode, typename TRule::OutNode, typename TFollowedBy::OutNode> Builder(
            const std::function<TOutNode(typename TRule::OutNode, typename TFollowedBy::OutNode)> &builder) {
        return {
            std::move(*this),
            builder
        };
    }

    template<class TRuleOut>
    ParserRuleNewFollowedBy<ParserRuleNewFollowedBy<TRule, TFollowedBy>, TRuleOut> FollowedBy(const TRuleOut &newFollowedBy) {
        return {*this, newFollowedBy};
    }

    ParserRuleNewFollowedBy<ParserRuleNewFollowedBy<TRule, TFollowedBy>, ParserRuleNewToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleNewToken{tokenType});
    }

    ParserRuleNewFollowedByIgnore<ParserRuleNewFollowedBy<TRule, TFollowedBy>, typename TRule::OutNode, typename TFollowedBy::OutNode> FollowedByIgnore(TokenType newToMatch) {
        return {std::move(*this), newToMatch};
    }

    std::optional<OutNode> Match(Parser &parser) const {
        std::optional<typename TRule::OutNode> ruleResult{ this->rule.Match(parser) };

        if (!ruleResult.has_value()) {
            return std::nullopt;
        }

        std::optional<typename TFollowedBy::OutNode> followedByResult{ this->followedBy.Match(parser) };

        if (!followedByResult.has_value()) {
            return std::nullopt;
        }

        return std::make_tuple(ruleResult.value(), followedByResult.value());
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleNewFollowedBy &ruleToOutput) {
        stream << ruleToOutput.rule << " followed by " << ruleToOutput.followedBy;
    }

private:
    TRule rule;
    TFollowedBy followedBy;
};

bool IsNextTokenMatch(Parser &parser, TokenType type);

template<class TRule, class ...TBuilderParams>
class ParserRuleNewFollowedByIgnore {
public:
    using OutNode = typename TRule::OutNode;

    ParserRuleNewFollowedByIgnore(const TRule &rule, TokenType toMatch) : rule{rule}, toMatch{toMatch} {}

    template<class TOutNode>
    NodeBuilder<ParserRuleNewFollowedByIgnore<TRule, TBuilderParams...>, TOutNode, TBuilderParams...> Builder(
            const std::function<TOutNode(TBuilderParams...)> &builder) {
        return {
                std::move(*this),
                builder
        };
    }

    template<class TRuleOut>
    ParserRuleNewFollowedBy<ParserRuleNewFollowedByIgnore<TRule, TBuilderParams...>, TRuleOut> FollowedBy(const TRuleOut &followedBy) {
        return {std::move(*this), followedBy};
    }

    ParserRuleNewFollowedByIgnore<ParserRuleNewFollowedByIgnore<TRule, TBuilderParams...>, TBuilderParams...> FollowedByIgnore(TokenType newToMatch) {
        return {std::move(*this), newToMatch};
    }

    ParserRuleNewFollowedBy<ParserRuleNewFollowedByIgnore<TRule, TBuilderParams...>, ParserRuleNewToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleNewToken{tokenType});
    }

    std::optional<OutNode> Match(Parser &parser) const {
        std::optional<typename TRule::OutNode> ruleResult{ this->rule.Match(parser) };
        bool isMatch{ IsNextTokenMatch(parser, this->toMatch) };

        if (!ruleResult.has_value() || !isMatch) {
            return std::nullopt;
        }

        return ruleResult.value();
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleNewFollowedByIgnore &ruleToOutput) {
        stream << ruleToOutput.rule << " followed by " << magic_enum::enum_name(ruleToOutput.toMatch);
    }

private:
    TRule rule;
    TokenType toMatch;
};


template <class TBuilder, class TOutNode, class ...TRest>
class ParserRuleNewBuilder {
public:
    explicit ParserRuleNewBuilder(const TBuilder &nodeBuilder) : nodeBuilder{nodeBuilder} {}

    template<class TFollowedBy>
    ParserRuleNewFollowedBy<TBuilder, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return ParserRuleNewFollowedBy<TBuilder, TFollowedBy>{this->nodeBuilder, followedBy};
    }

    ParserRuleNewFollowedBy<TBuilder, ParserRuleNewToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleNewToken{tokenType});
    }

    NodeBuilder<ParserRuleNewBuilder<TBuilder, TOutNode, TRest...>, TOutNode, TRest...> Builder(const std::function<TOutNode(TRest...)> &builder) {
        return NodeBuilder<ParserRuleNewBuilder<TBuilder, TOutNode, TRest...>, TOutNode>{
                std::move(*this),
                builder
        };
    }

    std::optional<typename TBuilder::OutNode> Match(Parser &parser) const {
        return this->nodeBuilder.Match(parser);
    }

private:
    const TBuilder &nodeBuilder;
};

template<class TNode>
class ParserRuleNewSurroundedBy {
public:
    ParserRuleNewSurroundedBy(ParserRuleNewToken left, ParserRuleNewToken right, std::optional<TNode> &out) : out{out} {}

private:
    ParserRuleNewToken left, right;
    std::optional<TNode> &out;
};

template<class TNode>
class ParserRuleNewNode {
public:
    ParserRuleNewNode(std::optional<TNode> &out) : out{out} {}

    ParserRuleNewSurroundedBy<TNode> SurroundedBy(TokenType left, TokenType right) {
        return ParserRuleNewSurroundedBy<TNode>{left, right, this->out};
    }
private:
    std::optional<TNode> &out;
};

#endif //JCC_PARSERRULEBUILDER_H
