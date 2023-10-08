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
            .Or(
                Parser::Expect<ASTExpression>()
                        .SurroundedBy(TokenType::LeftParenthesis, TokenType::RightParenthesis)
            )
            .Builder<ASTPrimaryExpression>([](std::variant<ASTIdentifier, ASTConstant, ASTExpression> &&variant) {
                if (std::holds_alternative<ASTExpression>(variant)) {
                    return ASTPrimaryExpression{ std::make_unique<ASTExpression>(std::move(std::get<ASTExpression>(variant))) };
                } else if (std::holds_alternative<ASTIdentifier>(variant)) {
                    return ASTPrimaryExpression{ std::get<ASTIdentifier>(variant) };
                } else if (std::holds_alternative<ASTConstant>(variant)) {
                    return ASTPrimaryExpression{ std::get<ASTConstant>(variant) };
                }

                assert(false);
            })
            .Match(parser);
}

std::string ASTPrimaryExpression::ToString(int depth) const {
    std::stringstream resultStream;
    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTPrimaryExpression {\n" << tabs;

    if (std::holds_alternative<ASTIdentifier>(this->inner)) {
        resultStream << std::get<ASTIdentifier>(this->inner).ToString(depth + 1);
    } else if (std::holds_alternative<ASTConstant>(this->inner)) {
        resultStream << std::get<ASTConstant>(this->inner).ToString(depth + 1);
    } else {
        resultStream << std::get<std::unique_ptr<ASTExpression>>(this->inner)->ToString(depth + 1);
    }

    resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
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
    using UnaryOperatorAndCast = std::tuple<ASTUnaryOperator, ASTCastExpression>;
    using Operation = std::variant<ASTUnaryIncrement, ASTUnaryDecrement>;

    return Parser::Expect<ASTUnaryIncrement>()
            .Or<ASTUnaryDecrement>()
            .ZeroOrMore()
            .FollowedBy<ASTPostfixExpression>()
            .Or(
                Parser::Expect<ASTUnaryOperator>()
                        .FollowedBy<ASTCastExpression>()
            )
            .Builder<ASTUnaryExpression>([](std::variant<std::tuple<std::vector<Operation>, ASTPostfixExpression>, UnaryOperatorAndCast>&& value) {
                if (std::holds_alternative<UnaryOperatorAndCast>(value)) {
                    auto &unaryOperatorAndCast = std::get<UnaryOperatorAndCast>(value);

                    return ASTUnaryExpression{
                            std::move(std::make_unique<ASTCastExpression>(std::move(std::get<1>(unaryOperatorAndCast)))),
                            std::move(std::get<0>(unaryOperatorAndCast))
                    };
                } else {
                    auto &unaryTuple = std::get<std::tuple<std::vector<Operation>, ASTPostfixExpression>>(value);
                    std::vector<Operation> &operations = std::get<0>(unaryTuple);
                    ASTPostfixExpression postfixExpression = std::move(std::get<1>(unaryTuple));

                    ASTUnaryExpression output{ std::move(postfixExpression), std::nullopt };

                    for (Operation &op : operations) {
                        if (std::holds_alternative<ASTUnaryIncrement>(op)) {
                            output = ASTUnaryExpression{ std::move(output), std::move(std::get<ASTUnaryIncrement>(op)) };
                        } else {
                            output = ASTUnaryExpression{ std::move(output), std::move(std::get<ASTUnaryDecrement>(op)) };
                        }
                    }

                    return output;
                }
            })
            .Match(parser);
}

std::string ASTUnaryExpression::ToString(int depth) const  {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTUnaryExpression {\n";
    resultStream << tabs << "inner = ";

    if (std::holds_alternative<std::unique_ptr<ASTUnaryExpression>>(inner)) {
        resultStream << std::get<std::unique_ptr<ASTUnaryExpression>>(inner)->ToString(depth + 1);
    } else if (std::holds_alternative<ASTPostfixExpression>(inner)) {
        resultStream << std::get<ASTPostfixExpression>(inner).ToString(depth + 1);
    } else {
        resultStream << std::get<std::unique_ptr<ASTCastExpression>>(inner)->ToString(depth + 1);
    }

    if (operation.has_value()) {
        resultStream << ",\n" << tabs << "operation = ";

        resultStream << std::visit([](const auto &op) {
            return op.ToString(0);
        }, operation.value());
    }
    resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

std::optional<ASTPostfixIndex> ASTPostfixIndex::Match(Parser &parser) {
    return Parser::Expect<ASTExpression>()
            .SurroundedBy(TokenType::LeftBracket, TokenType::RightBracket)
            .Builder<ASTPostfixIndex>([](ASTExpression &&expression) {
                return ASTPostfixIndex{ std::make_unique<ASTExpression>(std::move(expression)) };
            })
            .Match(parser);
}

std::string ASTPostfixIndex::ToString(int depth) const {
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
            .Builder<ASTPostfixArrow>([](const Token &, const ASTIdentifier &astIdentifier) {
                return ASTPostfixArrow{ astIdentifier };
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
    return Parser::Expect<ASTAdditiveExpression>()
            .FollowedBy(
            Parser::Expect(TokenType::Comma)
                        .FollowedBy<ASTAdditiveExpression>()
                        .ZeroOrMore()
            ).Builder<ASTExpression>([](ASTAdditiveExpression &&expr, std::vector<std::tuple<Token, ASTAdditiveExpression>> &&capturedExpressions) {
                std::vector<ASTAdditiveExpression> justTheExpressions{ };

                for (auto &item: capturedExpressions) {
                    justTheExpressions.push_back(std::move(std::get<1>(item)));
                }

                return ASTExpression{std::move(expr), std::move(justTheExpressions) };
           })
            .Match(parser);
}

std::string ASTExpression::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTExpression {\n";
    resultStream << tabs << "first = " << this->first.ToString(depth + 1) << ",\n";
    resultStream << tabs << "expressions = [";

    if (!this->expressions.empty()) {
        resultStream << '\n';
    }

    for (const auto &expression : this->expressions) {
        resultStream << tabs << expression.ToString(depth + 1) << ",\n";
    }

    if (!this->expressions.empty()) {
        resultStream << tabs;
    }

    resultStream << "]\n" << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

std::optional<ASTTypeQualifier> ASTTypeQualifier::Match(Parser &parser) {
    return Parser::Expect(TokenType::KeywordConst)
            .Or(TokenType::KeywordVolatile)
            .Or(TokenType::KeywordVolatile)
            .Or(TokenType::KeywordAtomic)
            .Builder<ASTTypeQualifier>([](const Token &token) {
                return ASTTypeQualifier{ ASTTypeQualifier::FromTokenType(token.type) };
            })
            .Match(parser);
}

std::optional<ASTTypeSpecifier> ASTTypeSpecifier::Match(Parser &parser) {
    return Parser::Expect(TokenType::KeywordVoid)
            .Or(TokenType::KeywordChar)
            .Or(TokenType::KeywordShort)
            .Or(TokenType::KeywordInt)
            .Or(TokenType::KeywordLong)
            .Or(TokenType::KeywordFloat)
            .Or(TokenType::KeywordDouble)
            .Or(TokenType::KeywordSigned)
            .Or(TokenType::KeywordUnsigned)
            .Or(TokenType::KeywordBool)
            .Or(TokenType::KeywordComplex)
            .Builder<ASTTypeSpecifier>([](const Token& token) {
                ASTTypeSpecifier::TypeSpecifier typeSpecifier{ ASTTypeSpecifier::FromTokenType(token.type) };

                return ASTTypeSpecifier{typeSpecifier};
            })
            .Match(parser);
}

std::optional<ASTSpecifierQualifierList> ASTSpecifierQualifierList::Match(Parser &parser) {
    return Parser::Expect<ASTTypeSpecifier>()
            .Or<ASTTypeQualifier>()
            .OneOrMore()
            .Builder<ASTSpecifierQualifierList>([](std::vector<std::variant<ASTTypeSpecifier, ASTTypeQualifier>> &&vec) {
                return ASTSpecifierQualifierList{ std::move(vec) };
            })
            .Match(parser);
}

std::string ASTSpecifierQualifierList::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);
    std::string tabs2(PRETTY_PRINT_DEPTH(1), PRETTY_PRINT_CHAR);

    resultStream << "ASTSpecifierQualifierList {\n";
    resultStream << tabs << "list = [";

    if (!this->list.empty()) {
        resultStream << '\n';
    }

    for (const std::variant<ASTTypeSpecifier, ASTTypeQualifier> &item : this->list) {
        resultStream << tabs << std::visit([&tabs2](const auto &item) {
            return tabs2 + item.ToString(0);
        }, item) << ",\n";
    }

    if (!this->list.empty()) {
        resultStream << tabs;
    }

    resultStream << "]\n" << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

std::optional<ASTTypeName> ASTTypeName::Match(Parser &parser) {
    return Parser::Expect<ASTSpecifierQualifierList>()
            .Builder<ASTTypeName>([](ASTSpecifierQualifierList &&list) {
                return ASTTypeName{ std::move(list) };
            })
            .Match(parser);
}

std::string ASTTypeName::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTTypeName {\n";
    resultStream << tabs << "specifierQualifierList = " << this->specifierQualifierList.ToString(depth + 1) << '\n';
    resultStream << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

std::optional<ASTCastExpression> ASTCastExpression::Match(Parser &parser) {
    return Parser::Expect<ASTTypeName>()
            .SurroundedBy(TokenType::LeftParenthesis, TokenType::RightParenthesis)
            .ZeroOrMore()
            .FollowedBy<ASTUnaryExpression>()
            .Builder<ASTCastExpression>([](std::vector<ASTTypeName> &&casts, ASTUnaryExpression &&unaryExpression) {
                ASTCastExpression output{ std::move(unaryExpression), std::nullopt };

                for (ASTTypeName &cast : casts) {
                    output = ASTCastExpression{ std::make_unique<ASTCastExpression>(std::move(output)), std::move(cast) };
                }

                return output;
            })
            .Match(parser);
}

std::string ASTCastExpression::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTCastExpression {\n";
    resultStream << tabs << "inner = ";

    if (std::holds_alternative<ASTUnaryExpression>(inner)) {
        resultStream << std::get<ASTUnaryExpression>(inner).ToString(depth + 1);
    } else {
        resultStream << std::get<std::unique_ptr<ASTCastExpression>>(inner)->ToString(depth + 1);
    }

    if (typeName.has_value()) {
        resultStream << ",\n" << tabs << "typeName = " << typeName.value().ToString(depth + 1);
    }
    resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

std::string ASTMultiplicativeExpression::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTMultiplicativeExpression {\n";

    resultStream << tabs << "lhs = ";

    if (this->lhs.has_value())
        resultStream << this->lhs->get()->ToString(depth + 1) << ",\n";
    else
        resultStream << "none,\n";

    resultStream << tabs << "rhs = ";

    if (this->rhs.has_value())
        resultStream << this->rhs->ToString(depth + 1) << ",\n";
    else
        resultStream << "none,\n";

    resultStream << tabs << "operator = " << magic_enum::enum_name(this->operatorType) << '\n';

    resultStream << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

std::optional<ASTMultiplicativeExpression> ASTMultiplicativeExpression::Match(Parser &parser) {
    return Parser::Expect<ASTCastExpression>()
            .FollowedBy(
                Parser::Expect(TokenType::Asterisk)
                    .Or(TokenType::Divide)
                    .Or(TokenType::Modulo)
            )
            .ZeroOrMore()
            .FollowedBy<ASTCastExpression>()
            .Builder<ASTMultiplicativeExpression>([](std::vector<std::tuple<ASTCastExpression, Token>> &&multiplicatives, ASTCastExpression &&rhs) {
                std::optional<std::unique_ptr<ASTMultiplicativeExpression>> output{ std::nullopt };

                for (auto &multiplicative : multiplicatives) {
                    ASTCastExpression &multLhs{std::get<0>(multiplicative) };
                    Token &opToken{ std::get<1>(multiplicative) };
                    MultiplicativeOperator op{ ASTMultiplicativeExpression::OperatorFromTokenType(opToken.type) };

                    output = std::make_unique<ASTMultiplicativeExpression>( std::move(output), op, std::move(multLhs) );
                }

                if (output.has_value()) {
                    ASTMultiplicativeExpression::MultiplicativeOperator opType{output.value()->operatorType};
                    std::unique_ptr<ASTMultiplicativeExpression> outputPtr{ std::move(output.value()) };

                    return ASTMultiplicativeExpression{ std::move(outputPtr), opType, std::move(rhs) };
                }

                return ASTMultiplicativeExpression{ std::nullopt, MultiplicativeOperator::None, std::move(rhs) };
            })
            .Match(parser);
}

ASTMultiplicativeExpression::MultiplicativeOperator
ASTMultiplicativeExpression::OperatorFromTokenType(TokenType tokenType) {
    {
        switch (tokenType) {
            case TokenType::Asterisk:
                return MultiplicativeOperator::Multiply;
            case TokenType::Divide:
                return MultiplicativeOperator::Divide;
            case TokenType::Modulo:
                return MultiplicativeOperator::Modulo;
            default:
                assert(false && "invalid token type");
                return MultiplicativeOperator::None;
        }
    }
}

std::string ASTAdditiveExpression::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTAdditiveExpression {\n";

    resultStream << tabs << "lhs = ";

    if (this->lhs.has_value())
        resultStream << this->lhs->get()->ToString(depth + 1) << ",\n";
    else
        resultStream << "none,\n";

    resultStream << tabs << "rhs = ";

    if (this->rhs.has_value())
        resultStream << this->rhs->ToString(depth + 1) << ",\n";
    else
        resultStream << "none,\n";

    resultStream << tabs << "operator = " << magic_enum::enum_name(this->operatorType) << '\n';

    resultStream << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

ASTAdditiveExpression::AdditiveOperator
ASTAdditiveExpression::OperatorFromTokenType(TokenType tokenType) {
    {
        switch (tokenType) {
            case TokenType::Plus:
                return AdditiveOperator::Add;
            case TokenType::Minus:
                return AdditiveOperator::Subtract;
            default:
                assert(false && "invalid token type");
                return AdditiveOperator::None;
        }
    }
}

// todo: same as multiplicative expression, refactor
std::optional<ASTAdditiveExpression> ASTAdditiveExpression::Match(Parser &parser) {
    return Parser::Expect<ASTMultiplicativeExpression>()
            .FollowedBy(
                    Parser::Expect(TokenType::Plus)
                            .Or(TokenType::Minus)
            )
            .ZeroOrMore()
            .FollowedBy<ASTMultiplicativeExpression>()
            .Builder<ASTAdditiveExpression>([](std::vector<std::tuple<ASTMultiplicativeExpression, Token>> &&multiplicatives, ASTMultiplicativeExpression &&rhs) {
                std::optional<std::unique_ptr<ASTAdditiveExpression>> output{ std::nullopt };

                for (auto &multiplicative : multiplicatives) {
                    ASTMultiplicativeExpression &multLhs{std::get<0>(multiplicative) };
                    Token &opToken{ std::get<1>(multiplicative) };
                    AdditiveOperator op{ ASTAdditiveExpression::OperatorFromTokenType(opToken.type) };

                    output = std::make_unique<ASTAdditiveExpression>( std::move(output), op, std::move(multLhs) );
                }

                if (output.has_value()) {
                    AdditiveOperator opType{output.value()->operatorType};
                    std::unique_ptr<ASTAdditiveExpression> outputPtr{ std::move(output.value()) };

                    return ASTAdditiveExpression{ std::move(outputPtr), opType, std::move(rhs) };
                }

                return ASTAdditiveExpression{ std::nullopt, AdditiveOperator::None, std::move(rhs) };
            })
            .Match(parser);
}

std::string ASTDeclaration::ToString(int depth) const {
    std::stringstream resultStream;

    resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth),     PRETTY_PRINT_CHAR)   << "ASTDeclaration {\n";
    resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "specifier  = " << this->specifier.ToString(depth + 1) << ",\n";
    resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "identifier = " << this->identifier.ToString(depth + 1) << ",\n";
    resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "expression = " << this->expression.ToString(depth + 1);
    resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth),     PRETTY_PRINT_CHAR)   << '}';

    return resultStream.str();
}
