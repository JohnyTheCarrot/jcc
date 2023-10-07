//
// Created by johny on 7/21/23.
//

#include "ASTNode.h"
#include "Parser.h"
#include <variant>

std::optional<ASTIdentifier> ASTIdentifier::Match(Parser &parser) {
    return Parser::Expect(TokenType::Identifier)
            .Builder<ASTIdentifier>([](Token token) {
                return ASTIdentifier{ std::get<std::string>(token.value) };
            })
            .Match(parser);
}

std::optional<ASTConstant> ASTConstant::Match(Parser &parser) {
    return Parser::Expect(TokenType::StringLiteral)
            .Or(TokenType::IntegerLiteral)
            .Or(TokenType::FloatLiteral)
            .Or(TokenType::DoubleLiteral)
            .Builder<ASTConstant>([](const Token& token) {
                return ASTConstant{ token };
            })
            .Match(parser);
}

std::optional<ASTPrimaryExpression> ASTPrimaryExpression::Match(Parser &parser) {
    return Parser::Expect<ASTIdentifier>()
            .Or<ASTConstant>()
            .Builder<ASTPrimaryExpression>([](const std::variant<ASTIdentifier, ASTConstant> &variant) {
                return ASTPrimaryExpression{ variant };
            })
            .Match(parser);
}

std::optional<ASTUnaryOperator> ASTUnaryOperator::Match(Parser &parser) {
    return Parser::Expect(TokenType::Plus)
            .Or(TokenType::Minus)
            .Or(TokenType::Ampersand)
            .Or(TokenType::Asterisk)
            .Or(TokenType::BitwiseNot)
            .Or(TokenType::LogicalNot)
            .Builder<ASTUnaryOperator>([](const Token &token) {
                return ASTUnaryOperator{ token.type };
            })
            .Match(parser);
}

std::optional<ASTUnaryIncrement> ASTUnaryIncrement::Match(Parser &parser) {
    return Parser::Expect(TokenType::Increment)
            .Builder<ASTUnaryIncrement>([](const Token &token) {
                return ASTUnaryIncrement{};
            })
            .Match(parser);
}

std::optional<ASTUnaryDecrement> ASTUnaryDecrement::Match(Parser &parser) {
    return Parser::Expect(TokenType::Decrement)
            .Builder<ASTUnaryDecrement>([](const Token &token) {
                return ASTUnaryDecrement{};
            })
            .Match(parser);
}

std::optional<ASTUnaryExpression> ASTUnaryExpression::Match(Parser &parser) {
    return Parser::Expect<ASTUnaryOperator>()
            .Or<ASTUnaryIncrement>()
            .Or<ASTUnaryDecrement>()
            .ZeroOrMore()
            .FollowedBy<ASTPostfixExpression>()
            .Builder<ASTUnaryExpression>([](std::vector<typename ASTUnaryExpression::Operator>&& operators, ASTPostfixExpression &&postfixExpressionNode) {
                ASTUnaryExpression output{std::move(postfixExpressionNode), std::nullopt};

                for (const typename ASTUnaryExpression::Operator &item : operators) {
                    output = ASTUnaryExpression{std::move(output), item};
                }

                return output;
            })
            .Match(parser);
}

std::optional<ASTPostfixIndex> ASTPostfixIndex::Match(Parser &parser) {
    return Parser::Expect(TokenType::LeftBracket)
            .FollowedBy<ASTExpression>()
            .FollowedByIgnore(TokenType::RightBracket)
            .Builder<ASTPostfixIndex>([](Token &&, ASTExpression &&expression) {
                return ASTPostfixIndex{ std::make_unique<ASTExpression>(std::move(expression)) };
            })
            .Match(parser);
}

std::string ASTPostfixIndex::ToString(int depth) const  {
    std::stringstream resultStream;

    resultStream << "ASTPostfixIndex {\n";

    resultStream << std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR) << "inner = " << this->inner->ToString(depth + 1) << '\n';

    resultStream << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

std::optional<ASTPostfixDot> ASTPostfixDot::Match(Parser &parser) {
    return Parser::Expect(TokenType::Dot)
            .FollowedBy<ASTIdentifier>()
            .Builder<ASTPostfixDot>([](const Token &, const ASTIdentifier &astIdentifier) {
                return ASTPostfixDot{ astIdentifier };
            })
            .Match(parser);
}

std::optional<ASTPostfixArrow> ASTPostfixArrow::Match(Parser &parser) {
    return Parser::Expect(TokenType::Arrow)
            .FollowedBy<ASTIdentifier>()
            .Builder<ASTPostfixArrow>([](const Token &, const ASTIdentifier &identifier) {
                return ASTPostfixArrow{ identifier };
            })
            .Match(parser);
}

std::optional<ASTPostfixDecrement> ASTPostfixDecrement::Match(Parser &parser) {
    return Parser::Expect(TokenType::Decrement)
            .Builder<ASTPostfixDecrement>([](const Token &token) {
                return ASTPostfixDecrement{};
            })
            .Match(parser);
}

std::optional<ASTPostfixIncrement> ASTPostfixIncrement::Match(Parser &parser) {
    return Parser::Expect(TokenType::Increment)
            .Builder<ASTPostfixIncrement>([](const Token &token) {
                return ASTPostfixIncrement{};
            })
            .Match(parser);
}

std::optional<ASTPostfixExpression> ASTPostfixExpression::Match(Parser &parser) {
    return Parser::Expect<ASTPrimaryExpression>()
            .FollowedBy(
                    Parser::Expect<ASTPostfixIncrement>()
                    .Or<ASTPostfixDecrement>()
                    .Or<ASTPostfixArrow>()
                    .Or<ASTPostfixDot>()
                    .Or<ASTPostfixIndex>()
                    .ZeroOrMore()
            ).Builder<ASTPostfixExpression>(
            [](
                    ASTPrimaryExpression &&primaryExpressionNode,
                    std::vector<typename ASTPostfixExpression::Operation> &&postfixExpressions
            ) {
                ASTPostfixExpression output{std::move(primaryExpressionNode), std::nullopt};

                for (typename ASTPostfixExpression::Operation &item: postfixExpressions) {
                    output = ASTPostfixExpression{std::move(output), std::move(item)};
                }

                return output;
            }
    ).Match(parser);
}

std::optional<ASTExpression> ASTExpression::Match(Parser &parser) {
    return Parser::Expect<ASTUnaryExpression>()
            .FollowedBy(
            Parser::Expect<ASTUnaryExpression>()
                        .ZeroOrMore()
            ).Builder<ASTExpression>([](ASTUnaryExpression &&unaryExpression, std::vector<ASTUnaryExpression> &&unaryExpressions) {
                return ASTExpression{ std::move(unaryExpression), std::move(unaryExpressions) };
           })
            .Match(parser);
}
