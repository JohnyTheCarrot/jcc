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

// https://stackoverflow.com/a/47204507/5119970
template <class... Args>
struct variant_cast_proxy
{
    std::variant<Args...> v;

    template <class... ToArgs>
    operator std::variant<ToArgs...>() const
    {
        return std::visit([](auto&& arg) -> std::variant<ToArgs...> { return arg ; },
                          v);
    }
};

template <class... Args>
auto variant_cast(const std::variant<Args...>& v) -> variant_cast_proxy<Args...>
{
    return {v};
}
// end of snippet from stackoverflow

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

    std::optional<TOutNode> Match(Parser &parser, bool shouldError = false) const {
        std::optional<typename TRule::OutNode> parseResult{this->rule.Match(parser)};

        if (!parseResult.has_value()) {
            if (!shouldError)
                return std::nullopt;

            std::stringstream errorMessage;
            errorMessage << "Expected " << this->rule << ".";

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

template <class TRule>
class ParserRuleZeroOrMore final {
public:
    using OutNode = std::vector<typename TRule::OutNode>;

    explicit ParserRuleZeroOrMore(const TRule &rule) : rule{rule} {}

    template<class TOutNode>
    NodeBuilder<ParserRuleZeroOrMore, TOutNode, OutNode> Builder(const std::function<TOutNode(const OutNode &)> &builder) {
        return {
                std::move(*this),
                builder
        };
    }

    std::optional<OutNode> Match(Parser &parser) const {
        std::vector<typename TRule::OutNode> result{};

        while (true) {
            std::optional<typename TRule::OutNode> ruleResult{ this->rule.Match(parser) };

            if (!ruleResult.has_value()) {
                break;
            }

            result.push_back(std::move(ruleResult.value()));
        }

        return result;
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleZeroOrMore &ruleToOutput) {
        stream << '(' << ruleToOutput.rule << ")*";

        return stream;
    }

private:
    TRule rule;
};

template <class TFirstOption, class TSecondOption>
class ParserRuleOr;

template<class TLeft, class TRight, class ...TLeftOptions>
class ParserRuleOrChain final {
public:
    using OutNode = std::variant<TLeftOptions..., typename TRight::OutNode>;
    using Self = ParserRuleOrChain<TLeft, TRight, TLeftOptions...>;

    ParserRuleOrChain(const TLeft &left, const TRight &right)
        : left{left}
        , right{right} {}

    template<class TOutNode>
    NodeBuilder<ParserRuleOrChain, TOutNode, OutNode> Builder(const std::function<TOutNode(const OutNode &)> &builder) {
        return {
                std::move(*this),
                builder
        };
    }

    template<class TAlternative>
    ParserRuleOrChain<Self, TAlternative, TLeftOptions..., typename TRight::OutNode> Or(const TAlternative &alternative) {
        return {*this, alternative};
    }

    std::optional<OutNode> Match(Parser &parser) const {
        std::optional<std::variant<TLeftOptions...>> leftResult{ this->left.Match(parser) };

        if (leftResult.has_value()) {
            // leftResult is a subset of OutNode, convert to OutNode
            return variant_cast(leftResult.value());
        }

        std::optional<typename TRight::OutNode> rightResult{ this->right.Match(parser) };

        if (rightResult.has_value()) {
            return rightResult.value();
        }

        return std::nullopt;
    }

    ParserRuleZeroOrMore<ParserRuleOrChain<TLeft, TRight, TLeftOptions...>> ZeroOrMore() const {
        return ParserRuleZeroOrMore<ParserRuleOrChain<TLeft, TRight, TLeftOptions...>>{*this};
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleOrChain &ruleToOutput) {
        stream << ruleToOutput.left << " or " << ruleToOutput.right;

        return stream;
    }

private:
    TLeft left;
    TRight right;
};

template <class TFirstOption, class TSecondOption>
class ParserRuleOr final {
public:
    using OutNode = std::variant<typename TFirstOption::OutNode, typename TSecondOption::OutNode>;

    using FirstOption = TFirstOption;
    using SecondOption = TSecondOption;

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

    template<class TAlternative>
    ParserRuleOrChain<ParserRuleOr<TFirstOption, TSecondOption>, TAlternative, typename TFirstOption::OutNode, typename TSecondOption::OutNode> Or(const TAlternative &alternative) {
        return {*this, alternative};
    }

    ParserRuleZeroOrMore<ParserRuleOr<TFirstOption, TSecondOption>> ZeroOrMore() {
        return ParserRuleZeroOrMore<ParserRuleOr<TFirstOption, TSecondOption>>{*this};
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

template<class TRule, class TFollowedBy>
class ParserRuleFollowedBy;

class ParserRuleToken {
public:
    using OutNode = Token;

    explicit ParserRuleToken(TokenType tokenType) : toMatch{tokenType} {}

    ParserRuleToken &Or(TokenType tokenType);

    template<class TFollowedBy>
    ParserRuleFollowedBy<ParserRuleToken, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return {std::move(*this), followedBy};
    }

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
        return this->nodeBuilder.Match(parser);
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
