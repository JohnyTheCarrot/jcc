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

template<class T>
auto variant_cast(T &&v) -> T {
    return std::forward<T>(v);
}


// https://stackoverflow.com/a/67106430/5119970
template<typename T, typename... Types>
constexpr bool are_types_unique_v = (!std::is_same_v<T, Types> && ...) && are_types_unique_v<Types...>;
template<typename T>
constexpr bool are_types_unique_v<T> = true;
// end of snippet from stackoverflow

template<typename T, typename... Types>
using MaybeVariant = std::conditional<are_types_unique_v<Types...> && sizeof...(Types) != 1, std::variant<Types...>, T>::type;

bool IsNextTokenMatch(Parser &parser, TokenType type);

int GetParserCursor(Parser &parser);

void SetParserCursor(Parser &parser, int cursor);

[[noreturn]]
void Error(Parser &parser, const std::string &message);

template<class TRule, class ...TRuleOutNode>
struct NodeBuilder {
    using BuilderFunction = std::function<Node(TRuleOutNode...)>;

    NodeBuilder(
            TRule &&rule,
            BuilderFunction &&builder
    ) : rule{std::move(rule)}
      , builder{std::move(builder)} {}

    std::optional<Node> Match(Parser &parser, bool shouldError = false) const {
        int cursor{ GetParserCursor(parser) };
        std::optional<typename TRule::OutNode> parseResult{this->rule.Match(parser)};

        if (!parseResult.has_value()) {
            if (!shouldError) {
                SetParserCursor(parser, cursor);
                return std::nullopt;
            }

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

template<class TRule, class TFollowedBy>
class ParserRuleFollowedBy;

template <class TFirstOption, class TSecondOption>
class ParserRuleOr;

template <class TRule>
class ParserRuleOneOrMore;

template <class TRule>
class ParserRuleZeroOrMore;

template<class TRule>
class ParserRuleSurroundedBy;

struct ParserRuleToken;

template<class TOut>
class ParserRuleFunction final {
public:
    using OutNode = Node;
    using OptionalOutNode = std::optional<OutNode>;
    using Self = ParserRuleFunction<TOut>;

    NodeBuilder<Self, OutNode> Builder(std::function<Node(OutNode &&)> &&builder) {
        return {
                std::move(*this),
                std::move(builder)
        };
    }

    template<class TAlternative>
    ParserRuleOr<Self, TAlternative> Or(const TAlternative &alternative) {
        return {*this, alternative};
    }

    template<class TAlternative>
    ParserRuleOr<Self, ParserRuleFunction<TAlternative>> Or() {
        return {*this, ParserRuleFunction<TAlternative>{}};
    }

    ParserRuleSurroundedBy<Self> SurroundedBy(TokenType left, TokenType right) {
        return {left, right, std::move(*this)};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return {*this, followedBy};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TFollowedBy>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TFollowedBy>{}};
    };

    ParserRuleOneOrMore<Self> OneOrMore() const {
        return ParserRuleOneOrMore<Self>{*this};
    }

    ParserRuleZeroOrMore<Self> ZeroOrMore() const {
        return ParserRuleZeroOrMore<Self>{*this};
    }

    OptionalOutNode Match(Parser &parser) const {
        return TOut::Match(parser);
    }

    friend std::ostream &operator<<(std::ostream &stream, const Self &ruleToOutput) {
        stream << "Function";

        return stream;
    }
};

template <class TRule>
class ParserRuleOneOrMore final {
public:
    using Self = ParserRuleOneOrMore<TRule>;
    using OutNode = std::vector<typename TRule::OutNode>;

    explicit ParserRuleOneOrMore(const TRule &rule) : rule{rule} {}

    NodeBuilder<Self, OutNode> Builder(std::function<Node(OutNode &&)> &&builder) {
        return {
                std::move(*this),
                std::move(builder)
        };
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return {std::move(*this), followedBy};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TFollowedBy>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TFollowedBy>{}};
    };

    std::optional<OutNode> Match(Parser &parser) const {
        std::vector<typename TRule::OutNode> result{};

        do {
            int cursor{ GetParserCursor(parser) };
            std::optional<typename TRule::OutNode> ruleResult{ this->rule.Match(parser) };

            if (!ruleResult.has_value()) {
                SetParserCursor(parser, cursor);
                break;
            }

            result.push_back(std::move(ruleResult.value()));
        } while (true);

        if (result.empty())
            return std::nullopt;

        return result;
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleOneOrMore &ruleToOutput) {
        stream << '(' << ruleToOutput.rule << ")+";

        return stream;
    }

private:
    TRule rule;
};

template <class TRule>
class ParserRuleZeroOrMore final {
public:
    using Self = ParserRuleZeroOrMore<TRule>;
    using OutNode = std::vector<typename TRule::OutNode>;

    explicit ParserRuleZeroOrMore(const TRule &rule) : rule{rule} {}

    template<class TOutNode>
    NodeBuilder<Self, OutNode> Builder(std::function<TOutNode(const OutNode &)> &&builder) {
        return {
                std::move(*this),
                std::move(builder)
        };
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return {std::move(*this), followedBy};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TFollowedBy>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TFollowedBy>{}};
    };

    std::optional<OutNode> Match(Parser &parser) const {
        std::vector<typename TRule::OutNode> result{};

        while (true) {
            int cursor{ GetParserCursor(parser) };
            std::optional<typename TRule::OutNode> ruleResult{ this->rule.Match(parser) };

            if (!ruleResult.has_value()) {
                SetParserCursor(parser, cursor);
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

template<class TLeft, class TRight, class TLeftFirst, class ...TLeftOptions>
class ParserRuleOrChain final {
public:
    using OutNode = MaybeVariant<typename TRight::OutNode, TLeftFirst, TLeftOptions...>;
    using Self = ParserRuleOrChain<TLeft, TRight, TLeftFirst, TLeftOptions...>;

    ParserRuleOrChain(const TLeft &left, const TRight &right)
        : left{left}
        , right{right} {}

    NodeBuilder<Self, OutNode> Builder(std::function<Node(OutNode &&)> &&builder) {
        return {
                std::move(*this),
                std::move(builder)
        };
    }

    template<class TAlternative>
    ParserRuleOrChain<Self, TAlternative, TLeftFirst, TLeftOptions..., typename TRight::OutNode> Or(const TAlternative &alternative) {
        return {*this, alternative};
    }

    template<class TAlternative>
    ParserRuleOrChain<Self, ParserRuleFunction<TAlternative>, TLeftFirst, TLeftOptions..., typename TRight::OutNode> Or() {
        return {*this, ParserRuleFunction<TAlternative>{}};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TFollowedBy>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TFollowedBy>{}};
    };

    std::optional<OutNode> Match(Parser &parser) const {
        int cursor{ GetParserCursor(parser) };
        std::optional<MaybeVariant<TLeftFirst, TLeftOptions...>> leftResult{ this->left.Match(parser) };

        if (leftResult.has_value()) {
            // leftResult is a subset of OutNode, convert to OutNode
            return std::move(variant_cast(leftResult.value()));
        }

        std::optional<typename TRight::OutNode> rightResult{ this->right.Match(parser) };

        if (rightResult.has_value()) {
            return std::move(rightResult.value());
        }

        SetParserCursor(parser, cursor);
        return std::nullopt;
    }

    ParserRuleOneOrMore<Self> OneOrMore() const {
        return ParserRuleOneOrMore<Self>{*this};
    }

    ParserRuleZeroOrMore<Self> ZeroOrMore() const {
        return ParserRuleZeroOrMore<Self>{*this};
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
    using Self = ParserRuleOr<TFirstOption, TSecondOption>;
    using OutNode = std::conditional<
            are_types_unique_v<typename TFirstOption::OutNode, typename TSecondOption::OutNode>,
            std::variant<typename TFirstOption::OutNode, typename TSecondOption::OutNode>,
            typename TFirstOption::OutNode
    >::type;
    using FirstOption = TFirstOption;
    using SecondOption = TSecondOption;

    ParserRuleOr(const TFirstOption &firstOption, const TSecondOption &secondOption)
        : firstOption{firstOption}
        , secondOption{secondOption} {}

    NodeBuilder<Self, OutNode> Builder(std::function<Node(OutNode &&)> &&builder) {
        return {
                std::move(*this),
                std::move(builder)
        };
    }

    template<class TAlternative>
    ParserRuleOrChain<Self, TAlternative, typename TFirstOption::OutNode, typename TSecondOption::OutNode> Or(const TAlternative &alternative) {
        return {*this, alternative};
    }

    template<class TAlternative>
    ParserRuleOrChain<Self, ParserRuleFunction<TAlternative>, typename TFirstOption::OutNode, typename TSecondOption::OutNode> Or() {
        return {*this, ParserRuleFunction<TAlternative>{}};
    }

    ParserRuleOneOrMore<Self> OneOrMore() const {
        return ParserRuleOneOrMore<Self>{*this};
    }

    ParserRuleZeroOrMore<Self> ZeroOrMore() {
        return ParserRuleZeroOrMore<Self>{*this};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return {std::move(*this), followedBy};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TFollowedBy>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TFollowedBy>{}};
    };

    std::optional<OutNode> Match(Parser &parser) const {
        int cursor{ GetParserCursor(parser) };
        std::optional<typename TFirstOption::OutNode> firstOptionResult{ this->firstOption.Match(parser) };

        if (firstOptionResult.has_value()) {
            return std::move(firstOptionResult.value());
        }

        std::optional<typename TSecondOption::OutNode> secondOptionResult{ this->secondOption.Match(parser) };

        if (secondOptionResult.has_value()) {
            return std::move(secondOptionResult.value());
        }

        SetParserCursor(parser, cursor);
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
    using Self = ParserRuleToken;
    using OutNode = Token;

    explicit ParserRuleToken(TokenType tokenType) : toMatch{tokenType} {}

    ParserRuleToken &Or(TokenType tokenType);

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return {std::move(*this), followedBy};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TFollowedBy>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TFollowedBy>{}};
    };

    NodeBuilder<ParserRuleToken, Token> Builder(std::function<Node(Token &&)> &&builder) {
        return {
                std::move(*this),
                std::forward<std::function<Node(Token)>>(builder)
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
    using Self = ParserRuleFollowedBy<TRule, TFollowedBy>;
    using OutNode = std::tuple<typename TRule::OutNode, typename TFollowedBy::OutNode>;

    ParserRuleFollowedBy(const TRule &rule, const TFollowedBy &followedBy) : rule{rule}, followedBy{followedBy} {}

    NodeBuilder<Self, typename TRule::OutNode, typename TFollowedBy::OutNode> Builder(
            std::function<Node(typename TRule::OutNode &&, typename TFollowedBy::OutNode &&)> &&builder) {
        return {
            std::move(*this),
            std::move(builder)
        };
    }

    template<class TRuleOut>
    ParserRuleFollowedBy<Self, TRuleOut> FollowedBy(const TRuleOut &newFollowedBy) {
        return {*this, newFollowedBy};
    }

    ParserRuleFollowedBy<Self, ParserRuleToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleToken{tokenType});
    }

    ParserRuleZeroOrMore<Self> ZeroOrMore() const {
        return ParserRuleZeroOrMore<Self>{*this};
    }

    template<class TAlternative>
    ParserRuleOr<Self, TAlternative> Or(const TAlternative &alternative) {
        return {*this, alternative};
    }

    template<class TAlternative>
    ParserRuleOr<Self, ParserRuleFunction<TAlternative>> Or() {
        return {*this, ParserRuleFunction<TAlternative>{}};
    }

    ParserRuleFollowedByIgnore<Self, typename TRule::OutNode, typename TFollowedBy::OutNode> FollowedByIgnore(TokenType newToMatch) {
        return {std::move(*this), newToMatch};
    }

    std::optional<OutNode> Match(Parser &parser) const {
        int cursor{ GetParserCursor(parser) };
        std::optional<typename TRule::OutNode> ruleResult{ this->rule.Match(parser) };

        if (!ruleResult.has_value()) {
            SetParserCursor(parser, cursor);
            return std::nullopt;
        }

        std::optional<typename TFollowedBy::OutNode> followedByResult{ this->followedBy.Match(parser) };

        if (!followedByResult.has_value()) {
            SetParserCursor(parser, cursor);
            return std::nullopt;
        }

        return std::make_tuple(std::move(ruleResult.value()), std::move(followedByResult.value()));
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleFollowedBy &ruleToOutput) {
        stream << ruleToOutput.rule << " followed by " << ruleToOutput.followedBy;

        return stream;
    }

private:
    TRule rule;
    TFollowedBy followedBy;
};

template<class TRule, class ...TBuilderParams>
class ParserRuleFollowedByIgnore {
public:
    using Self = ParserRuleFollowedByIgnore<TRule, TBuilderParams...>;
    using OutNode = typename TRule::OutNode;

    ParserRuleFollowedByIgnore(TRule &&rule, TokenType toMatch) : rule{std::move(rule)}, toMatch{toMatch} {}

    NodeBuilder<Self, TBuilderParams...> Builder(
            std::function<Node(TBuilderParams...)> &&builder) {
        return {
                std::move(*this),
                std::move(builder)
        };
    }

    template<class TRuleOut>
    ParserRuleFollowedBy<Self, TRuleOut> FollowedBy(const TRuleOut &followedBy) {
        return {std::move(*this), followedBy};
    }

    template<class TRuleOut>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TRuleOut>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TRuleOut>{}};
    }

    ParserRuleFollowedByIgnore<Self, TBuilderParams...> FollowedByIgnore(TokenType newToMatch) {
        return {std::move(*this), newToMatch};
    }

    ParserRuleFollowedBy<Self, ParserRuleToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleToken{tokenType});
    }

    std::optional<OutNode> Match(Parser &parser) const {
        int cursor{ GetParserCursor(parser) };
        std::optional<typename TRule::OutNode> ruleResult{ this->rule.Match(parser) };
        bool isMatch{ IsNextTokenMatch(parser, this->toMatch) };

        if (!ruleResult.has_value() || !isMatch) {
            SetParserCursor(parser, cursor);
            return std::nullopt;
        }

        return std::move(ruleResult.value());
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

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TFollowedBy>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TFollowedBy>{}};
    };

    ParserRuleFollowedBy<TBuilder, ParserRuleToken> FollowedBy(TokenType tokenType) {
        return this->FollowedBy(ParserRuleToken{tokenType});
    }

    template<class TAlternative>
    ParserRuleOr<Self, TAlternative> Or(const TAlternative &alternative) {
        return {*this, alternative};
    }

    ParserRuleOneOrMore<Self> OneOrMore() const {
        return ParserRuleOneOrMore<Self>{*this};
    }

    ParserRuleZeroOrMore<Self> ZeroOrMore() const {
        return ParserRuleZeroOrMore<Self>{*this};
    }

    NodeBuilder<Self, TRest...> Builder(std::function<TOutNode(TRest...)> &&builder) {
        return {
                std::move(*this),
                std::move(builder)
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
    TBuilder nodeBuilder;
};

template<class TRule>
class ParserRuleSurroundedBy {
public:
    using Self = ParserRuleSurroundedBy<TRule>;
    using OutNode = typename TRule::OutNode;

    ParserRuleSurroundedBy(TokenType left, TokenType &right, TRule &&out)
        : left{left}
        , right{right}
        , out{out} {}

    NodeBuilder<Self, typename TRule::OutNode> Builder(std::function<Node(typename TRule::OutNode&&)> &&builder) {
        return {
                std::move(*this),
                std::move(builder)
        };
    }

    ParserRuleOneOrMore<Self> OneOrMore() const {
        return ParserRuleOneOrMore<Self>{*this};
    }

    ParserRuleZeroOrMore<Self> ZeroOrMore() const {
        return ParserRuleZeroOrMore<Self>{*this};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, TFollowedBy> FollowedBy(const TFollowedBy &followedBy) {
        return {*this, followedBy};
    }

    template<class TFollowedBy>
    ParserRuleFollowedBy<Self, ParserRuleFunction<TFollowedBy>> FollowedBy() {
        return {std::move(*this), ParserRuleFunction<TFollowedBy>{}};
    };

    std::optional<OutNode> Match(Parser &parser) const {
        int cursor{ GetParserCursor(parser) };
        bool leftIsMatch{ IsNextTokenMatch(parser, this->left) };

        if (!leftIsMatch) {
            SetParserCursor(parser, cursor);
            return std::nullopt;
        }

        std::optional<typename TRule::OutNode> outResult{ this->out.Match(parser) };

        if (!outResult.has_value()) {
            SetParserCursor(parser, cursor);
            return std::nullopt;
        }

        bool rightIsMatch{ IsNextTokenMatch(parser, this->right) };

        if (!rightIsMatch) {
            SetParserCursor(parser, cursor);
            return std::nullopt;
        }

        return std::move(outResult.value());
    }

    friend std::ostream &operator<<(std::ostream &stream, const ParserRuleSurroundedBy &ruleToOutput) {
        stream << magic_enum::enum_name(ruleToOutput.left) << " " << ruleToOutput.out << " " << magic_enum::enum_name(ruleToOutput.right);

        return stream;
    }

private:
    TokenType left, right;
    TRule &out;
};

#endif //JCC_PARSERRULEBUILDER_H
