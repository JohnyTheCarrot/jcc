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
            errorMessage << "Expected " << this->rule << ".";

            Error(parser, errorMessage.str());
        }

        return std::apply(this->builder, std::tuple(std::move(parseResult.value())));
    }

    std::optional<TOutNode> MatchOptional(Parser &parser) const {
        std::optional<typename TRule::OutNode> parseResult{this->rule.Match(parser)};

        if (!parseResult.has_value()) {
            return std::nullopt;
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

template <class TFirstOption, class TSecondOption>
class ParserRuleOr {
public:
    using OutNode = std::variant<typename TFirstOption::OutNode, typename TSecondOption::OutNode>;

    ParserRuleOr(const TFirstOption &firstOption, const TSecondOption &secondOption)
        : firstOption{firstOption}
        , secondOption{secondOption} {}

    template<class TOutNode>
    NodeBuilder<ParserRuleOr, TOutNode, OutNode> Builder(const std::function<TOutNode(const OutNode &)> &builder) {
        return {
                std::move(*this),
                builder
        };
    }

    std::optional<OutNode> Match(Parser &parser) const {
        std::optional<typename TFirstOption::OutNode> firstOptionResult{ this->firstOption.Match(parser) };

        if (firstOptionResult.has_value()) {
            return std::move(firstOptionResult.value());
        }

        std::optional<typename TSecondOption::OutNode> secondOptionResult{ this->secondOption.Match(parser) };

        if (secondOptionResult.has_value()) {
            return std::move(secondOptionResult.value());
        }

        return std::nullopt;
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleOr &ruleToOutput) {
        stream << ruleToOutput.firstOption << " or " << ruleToOutput.secondOption;

        return stream;
    }

private:
    TFirstOption firstOption;
    TSecondOption secondOption;
};

class ParserRuleToken {
public:
    using OutNode = Token;

    explicit ParserRuleToken(TokenType tokenType) : toMatch{tokenType} {}

    ParserRuleToken &Or(TokenType tokenType);

    template<class TOutNode>
    NodeBuilder<ParserRuleToken, TOutNode, Token> Builder(const std::function<TOutNode(Token)> &builder) {
        return {
                std::move(*this),
                builder
        };
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleToken &ruleToOutput) {
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
class ParserRuleFollowedByIgnore;

template<class TRule, class TFollowedBy>
class ParserRuleFollowedBy {
public:
    using OutNode = std::tuple<typename TRule::OutNode, typename TFollowedBy::OutNode>;

    ParserRuleFollowedBy(const TRule &rule, const TFollowedBy &followedBy) : rule{rule}, followedBy{followedBy} {}

    template<class TOutNode>
    NodeBuilder<ParserRuleFollowedBy, TOutNode, typename TRule::OutNode, typename TFollowedBy::OutNode> Builder(
            const std::function<TOutNode(typename TRule::OutNode, typename TFollowedBy::OutNode)> &builder) {
        return {
            std::move(*this),
            builder
        };
    }

    template<class TRuleOut>
    ParserRuleFollowedBy<ParserRuleFollowedBy<TRule, TFollowedBy>, TRuleOut> FollowedBy(const TRuleOut &newFollowedBy) {
        return {*this, newFollowedBy};
    }

    ParserRuleFollowedBy<ParserRuleFollowedBy<TRule, TFollowedBy>, ParserRuleToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleToken{tokenType});
    }

    ParserRuleFollowedByIgnore<ParserRuleFollowedBy<TRule, TFollowedBy>, typename TRule::OutNode, typename TFollowedBy::OutNode> FollowedByIgnore(TokenType newToMatch) {
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

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleFollowedBy &ruleToOutput) {
        stream << ruleToOutput.rule << " followed by " << ruleToOutput.followedBy;

        return stream;
    }

private:
    TRule rule;
    TFollowedBy followedBy;
};

bool IsNextTokenMatch(Parser &parser, TokenType type);

template<class TRule, class ...TBuilderParams>
class ParserRuleFollowedByIgnore {
public:
    using OutNode = typename TRule::OutNode;

    ParserRuleFollowedByIgnore(const TRule &rule, TokenType toMatch) : rule{rule}, toMatch{toMatch} {}

    template<class TOutNode>
    NodeBuilder<ParserRuleFollowedByIgnore<TRule, TBuilderParams...>, TOutNode, TBuilderParams...> Builder(
            const std::function<TOutNode(TBuilderParams...)> &builder) {
        return {
                std::move(*this),
                builder
        };
    }

    template<class TRuleOut>
    ParserRuleFollowedBy<ParserRuleFollowedByIgnore<TRule, TBuilderParams...>, TRuleOut> FollowedBy(const TRuleOut &followedBy) {
        return {std::move(*this), followedBy};
    }

    ParserRuleFollowedByIgnore<ParserRuleFollowedByIgnore<TRule, TBuilderParams...>, TBuilderParams...> FollowedByIgnore(TokenType newToMatch) {
        return {std::move(*this), newToMatch};
    }

    ParserRuleFollowedBy<ParserRuleFollowedByIgnore<TRule, TBuilderParams...>, ParserRuleToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleToken{tokenType});
    }

    std::optional<OutNode> Match(Parser &parser) const {
        std::optional<typename TRule::OutNode> ruleResult{ this->rule.Match(parser) };
        bool isMatch{ IsNextTokenMatch(parser, this->toMatch) };

        if (!ruleResult.has_value() || !isMatch) {
            return std::nullopt;
        }

        return ruleResult.value();
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleFollowedByIgnore &ruleToOutput) {
        stream << ruleToOutput.rule << " followed by " << magic_enum::enum_name(ruleToOutput.toMatch);

        return stream;
    }

private:
    TRule rule;
    TokenType toMatch;
};


template <class TBuilder, class TOutNode, class ...TRest>
class ParserRuleBuilder {
public:
    using Self = ParserRuleBuilder<TBuilder, TOutNode, TRest...>;
    using OutNode = typename TBuilder::OutNode;

    explicit ParserRuleBuilder(const TBuilder &nodeBuilder) : nodeBuilder{nodeBuilder} {}

    template<class TFollowedBy>
    ParserRuleFollowedBy<TBuilder, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return {this->nodeBuilder, followedBy};
    }

    ParserRuleFollowedBy<TBuilder, ParserRuleToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleToken{tokenType});
    }

    template<class TAlternative>
    ParserRuleOr<Self, TAlternative> Or(const TAlternative &alternative) {
        return {*this, alternative};
    }

    NodeBuilder<ParserRuleBuilder<TBuilder, TOutNode, TRest...>, TOutNode, TRest...> Builder(const std::function<TOutNode(TRest...)> &builder) {
        return {
                std::move(*this),
                builder
        };
    }

    std::optional<typename TBuilder::OutNode> Match(Parser &parser) const {
        return this->nodeBuilder.MatchOptional(parser);
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleBuilder &ruleToOutput) {
        stream << ruleToOutput.nodeBuilder;

        return stream;
    }

private:
    const TBuilder &nodeBuilder;
};

template<class TNode>
class ParserRuleSurroundedBy {
public:
    ParserRuleSurroundedBy(ParserRuleToken left, ParserRuleToken right, std::optional<TNode> &out) : out{out} {}

private:
    ParserRuleToken left, right;
    std::optional<TNode> &out;
};

template<class TNode>
class ParserRuleNode {
public:
    ParserRuleNode(std::optional<TNode> &out) : out{out} {}

    ParserRuleSurroundedBy<TNode> SurroundedBy(TokenType left, TokenType right) {
        return ParserRuleSurroundedBy<TNode>{left, right, this->out};
    }
private:
    std::optional<TNode> &out;
};

#endif //JCC_PARSERRULEBUILDER_H
