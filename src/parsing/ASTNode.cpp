//
// Created by johny on 7/21/23.
//

#include "ASTNode.h"
#include "Parser.h"
#include <variant>

std::optional<Node> ASTIdentifier::Match(Parser &parser) {
    return Parser::Expect(TokenType::Identifier)
            .Builder([](Token &&token) -> Node {
                return ASTIdentifier{ std::get<std::string>(token.value) }.ToNode();
            })
            .Match(parser);
}

Node ASTIdentifier::ToNode() {
    return Node {
        Node::Type::Identifier,
        std::make_unique<ASTIdentifier>(std::move(*this))
    };
}

std::optional<Node> ASTConstant::Match(Parser &parser) {
    return Parser::Expect(TokenType::StringLiteral)
            .Or(TokenType::IntegerLiteral)
            .Or(TokenType::FloatLiteral)
            .Or(TokenType::DoubleLiteral)
            .Builder([](const Token &token) -> Node {
                return ASTConstant{ token }.ToNode();
            })
            .Match(parser);
}

std::string ASTConstant::ToString(int depth) const {
    std::stringstream resultStream;

    resultStream << "ASTConstant(";

    IntegerLiteralTokenValue tokenValue;

    switch (this->constantToken.type) {
        case TokenType::StringLiteral:
            resultStream << '"' << std::get<std::string>(this->constantToken.value) << '"';
            break;
        case TokenType::IntegerLiteral:
            tokenValue = std::get<IntegerLiteralTokenValue>(this->constantToken.value);
            resultStream << std::to_string(tokenValue.value);
            if (tokenValue.isUnsigned)
                resultStream << 'u';

            switch (tokenValue.type) {
                case IntegerLiteralType::Int:
                    break;
                case IntegerLiteralType::Long:
                    resultStream << 'l';
                    break;
                case IntegerLiteralType::LongLong:
                    resultStream << "ll";
                    break;
            }
            break;
        case TokenType::DoubleLiteral:
            resultStream << std::to_string(std::get<double>(this->constantToken.value));
            break;
        default:
            assert(false);
    }

    resultStream << ')';

    return resultStream.str();
}

Node ASTConstant::ToNode() {
    return Node {
        Node::Type::Constant,
        std::make_unique<ASTConstant>(std::move(*this))
    };
}

std::optional<Node> ASTPrimaryExpression::Match(Parser &parser) {
    return Parser::Expect<ASTIdentifier>()
            .Or<ASTConstant>()
            .Or(
                Parser::Expect<ASTExpression>()
                        .SurroundedBy(TokenType::LeftParenthesis, TokenType::RightParenthesis)
            )
            .Builder(std::function([](Node &&variant) -> Node {
                return std::move(variant);
            }))
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

Node ASTPrimaryExpression::ToNode() {
    return Node {
        Node::Type::PrimaryExpression,
        std::make_unique<ASTPrimaryExpression>(std::move(*this))
    };
}

std::optional<Node> ASTUnaryOperator::Match(Parser &parser) {
    return Parser::Expect(TokenType::Plus)
            .Or(TokenType::Minus)
            .Or(TokenType::Ampersand)
            .Or(TokenType::Asterisk)
            .Or(TokenType::BitwiseNot)
            .Or(TokenType::LogicalNot)
            .Builder([](const Token &token) -> Node {
                return ASTUnaryOperator{ token.type }.ToNode();
            })
            .Match(parser);
}

ASTUnaryOperator::ASTUnaryOperator(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::Ampersand:
            this->unaryOperator = UnaryOperator::Reference;
            break;
        case TokenType::Asterisk:
            this->unaryOperator = UnaryOperator::Dereference;
            break;
        case TokenType::Plus:
            this->unaryOperator = UnaryOperator::Positive;
            break;
        case TokenType::Minus:
            this->unaryOperator = UnaryOperator::Negative;
            break;
        case TokenType::BitwiseNot:
            this->unaryOperator = UnaryOperator::BitwiseNot;
            break;
        case TokenType::LogicalNot:
            this->unaryOperator = UnaryOperator::Not;
            break;
        default:
            assert(false);
    }
}

std::string ASTUnaryOperator::ToString(int depth) const {
    std::stringstream resultStream;

    resultStream << "ASTUnaryOperator(" << magic_enum::enum_name(this->unaryOperator) << ")";

    return resultStream.str();
}

Node ASTUnaryOperator::ToNode() {
    return Node {
        Node::Type::UnaryOperator,
        std::make_unique<ASTUnaryOperator>(std::move(*this))
    };
}

std::optional<Node> ASTUnaryIncrement::Match(Parser &parser) {
    return Parser::Expect(TokenType::Increment)
            .Builder([](const Token &token) -> Node {
                return ASTUnaryIncrement{}.ToNode();
            })
            .Match(parser);
}

Node ASTUnaryIncrement::ToNode() {
    return Node {
        Node::Type::UnaryIncrement,
        std::make_unique<ASTUnaryIncrement>(std::move(*this))
    };
}

std::optional<Node> ASTUnaryDecrement::Match(Parser &parser) {
    return Parser::Expect(TokenType::Decrement)
            .Builder([](const Token &token) -> Node {
                return ASTUnaryDecrement{}.ToNode();
            })
            .Match(parser);
}

Node ASTUnaryDecrement::ToNode() {
    return Node {
        Node::Type::UnaryDecrement,
        std::make_unique<ASTUnaryDecrement>(std::move(*this))
    };
}

std::optional<Node> ASTUnaryExpression::Match(Parser &parser) {
    using UnaryOperatorAndCast = std::tuple<Node, Node>;
    using Operation = Node;

    return Parser::Expect<ASTUnaryIncrement>()
            .Or<ASTUnaryDecrement>()
            .ZeroOrMore()
            .FollowedBy<ASTPostfixExpression>()
            .Or(
                Parser::Expect<ASTUnaryOperator>()
                        .FollowedBy<ASTCastExpression>()
            )
            .Builder([](std::variant<std::tuple<std::vector<Operation>, Node>, UnaryOperatorAndCast>&& value) {
                if (std::holds_alternative<UnaryOperatorAndCast>(value)) {
                    auto &unaryOperatorAndCast = std::get<UnaryOperatorAndCast>(value);

                    return ASTUnaryExpression{
                            std::move(std::move(std::get<1>(unaryOperatorAndCast))),
                            std::move(std::get<0>(unaryOperatorAndCast))
                    }.ToNode();
                } else {
                    auto &unaryTuple = std::get<std::tuple<std::vector<Operation>, Node>>(value);
                    std::vector<Operation> &operations = std::get<0>(unaryTuple);
                    Node postfixExpression = std::move(std::get<1>(unaryTuple));

                    Node output{ std::move(postfixExpression) };

                    if (operations.empty())
                        return output;

                    for (Operation &op : operations) {
                        output = ASTUnaryExpression{ std::move(output), std::move(op) }.ToNode();
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

    resultStream << this->inner.ToString(depth + 1);

    resultStream << ",\n" << tabs << "operation = ";

    resultStream << operation.ToString(depth + 1);
    resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

Node ASTUnaryExpression::ToNode() {
    return Node {
        Node::Type::UnaryExpression,
        std::make_unique<ASTUnaryExpression>(std::move(*this))
    };
}

std::optional<Node> ASTPostfixIndex::Match(Parser &parser) {
    return Parser::Expect<ASTExpression>()
            .SurroundedBy(TokenType::LeftBracket, TokenType::RightBracket)
            .Builder([](Node &&expression) -> Node {
                return ASTPostfixIndex{ std::move(expression) }.ToNode();
            })
            .Match(parser);
}

std::string ASTPostfixIndex::ToString(int depth) const {
    std::stringstream resultStream;

    resultStream << "ASTPostfixIndex {\n";

    resultStream << std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR) << "inner = " << this->inner.ToString(depth + 1) << '\n';

    resultStream << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

Node ASTPostfixIndex::ToNode() {
    return Node {
        Node::Type::PostfixIndex,
        std::make_unique<ASTPostfixIndex>(std::move(*this))
    };
}

std::optional<Node> ASTPostfixDot::Match(Parser &parser) {
    return Parser::Expect(TokenType::Dot)
            .FollowedBy<ASTIdentifier>()
            .Builder([](Token &&, Node &&astIdentifier) -> Node {
                return ASTPostfixDot{ std::move(astIdentifier) }.ToNode();
            })
            .Match(parser);
}

Node ASTPostfixDot::ToNode() {
    return Node {
        Node::Type::PostfixDot,
        std::make_unique<ASTPostfixDot>(std::move(*this))
    };
}

std::optional<Node> ASTPostfixArrow::Match(Parser &parser) {
    return Parser::Expect(TokenType::Arrow)
            .FollowedBy<ASTIdentifier>()
            .Builder([](Token &&, Node &&astIdentifier) -> Node {
                return ASTPostfixArrow{ std::move(astIdentifier) }.ToNode();
            })
            .Match(parser);
}

Node ASTPostfixArrow::ToNode() {
    return Node {
        Node::Type::PostfixArrow,
        std::make_unique<ASTPostfixArrow>(std::move(*this))
    };
}

std::optional<Node> ASTPostfixDecrement::Match(Parser &parser) {
    return Parser::Expect(TokenType::Decrement)
            .Builder([](Token &&token) -> Node {
                return ASTPostfixDecrement{}.ToNode();
            })
            .Match(parser);
}

Node ASTPostfixDecrement::ToNode() {
    return Node {
        Node::Type::PostfixDecrement,
        std::make_unique<ASTPostfixDecrement>(std::move(*this))
    };
}

std::optional<Node> ASTPostfixIncrement::Match(Parser &parser) {
    return Parser::Expect(TokenType::Increment)
            .Builder([](Token &&token) -> Node {
                return ASTPostfixIncrement{}.ToNode();
            })
            .Match(parser);
}

Node ASTPostfixIncrement::ToNode() {
    return Node {
        Node::Type::PostfixIncrement,
        std::make_unique<ASTPostfixIncrement>(std::move(*this))
    };
}

std::optional<Node> ASTPostfixExpression::Match(Parser &parser) {
    return Parser::Expect<ASTPrimaryExpression>()
            .FollowedBy(
                Parser::Expect<ASTPostfixIncrement>()
                    .Or<ASTPostfixDecrement>()
                    .Or<ASTPostfixArrow>()
                    .Or<ASTPostfixDot>()
                    .Or<ASTPostfixIndex>()
                    .ZeroOrMore()
            ).Builder(
            [](
                    Node &&primaryExpressionNode,
                    std::vector<Node> &&postfixExpressions
            ) -> Node {
                if (postfixExpressions.empty()) {
                    return std::move(primaryExpressionNode);
                }

                Node output{ std::move(primaryExpressionNode) };

                for (Node &item : postfixExpressions) {
                    output = ASTPostfixExpression{std::move(output), std::move(item)}.ToNode();
                }

                return output;
            })
            .Match(parser);
}

std::string ASTPostfixExpression::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTPostfixExpression {\n";
    resultStream << tabs << "inner = " << this->inner.ToString(depth + 1) << ",\n" << tabs << "operation = ";

    resultStream << operation.ToString(depth + 1);
    resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

Node ASTPostfixExpression::ToNode() {
    return Node {
        Node::Type::PostfixExpression,
        std::make_unique<ASTPostfixExpression>(std::move(*this))
    };
}

std::optional<Node> ASTExpression::Match(Parser &parser) {
    return Parser::Expect<ASTAdditiveExpression>()
            .FollowedBy(
            Parser::Expect(TokenType::Comma)
                        .FollowedBy<ASTAdditiveExpression>()
                        .ZeroOrMore()
            ).Builder([](Node &&expr, std::vector<std::tuple<Token, Node>> &&capturedExpressions) -> Node {
                if (capturedExpressions.empty())
                    return std::move(expr);

                std::vector<Node> justTheExpressions{ };

                for (auto &item: capturedExpressions) {
                    justTheExpressions.push_back(std::move(std::get<1>(item)));
                }

                return ASTExpression{std::move(expr), std::move(justTheExpressions) }.ToNode();
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

Node ASTExpression::ToNode() {
    return Node {
        Node::Type::Expression,
        std::make_unique<ASTExpression>(std::move(*this))
    };
}

std::optional<Node> ASTTypeQualifier::Match(Parser &parser) {
    return Parser::Expect(TokenType::KeywordConst)
            .Or(TokenType::KeywordVolatile)
            .Or(TokenType::KeywordVolatile)
            .Or(TokenType::KeywordAtomic)
            .Builder([](Token &&token) -> Node {
                return ASTTypeQualifier{ ASTTypeQualifier::FromTokenType(token.type) }.ToNode();
            })
            .Match(parser);
}

Node ASTTypeQualifier::ToNode() {
    return Node {
        Node::Type::TypeQualifier,
        std::make_unique<ASTTypeQualifier>(std::move(*this))
    };
}

std::optional<Node> ASTTypeSpecifier::Match(Parser &parser) {
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
            .Builder([](Token &&token) -> Node {
                ASTTypeSpecifier::TypeSpecifier typeSpecifier{ ASTTypeSpecifier::FromTokenType(token.type) };

                return ASTTypeSpecifier{typeSpecifier}.ToNode();
            })
            .Match(parser);
}

Node ASTTypeSpecifier::ToNode() {
    return Node {
        Node::Type::TypeSpecifier,
        std::make_unique<ASTTypeSpecifier>(std::move(*this))
    };
}

std::string ASTTypeSpecifier::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR);

    resultStream << tabs << "ASTTypeSpecifier(" << magic_enum::enum_name(this->specifier) << ')';

    return resultStream.str();
}

std::optional<Node> ASTSpecifierQualifierList::Match(Parser &parser) {
    return Parser::Expect<ASTTypeSpecifier>()
            .Or<ASTTypeQualifier>()
            .OneOrMore()
            .Builder([](std::vector<Node> &&vec) -> Node {
                return ASTSpecifierQualifierList{ std::move(vec) }.ToNode();
            })
            .Match(parser);
}

std::string ASTSpecifierQualifierList::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);
    std::string tabs2(PRETTY_PRINT_DEPTH(depth + 2), PRETTY_PRINT_CHAR);

    resultStream << "ASTSpecifierQualifierList {\n";
    resultStream << tabs << "list = [";

    if (!this->list.empty()) {
        resultStream << '\n';
    }

    for (const Node &node : this->list) {
        resultStream << node.ToString(depth + 2) << ",\n";
    }

    if (!this->list.empty()) {
        resultStream << tabs;
    }

    resultStream << "]\n" << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

Node ASTSpecifierQualifierList::ToNode() {
    return Node{
        Node::Type::SpecifierQualifierList,
        std::make_unique<ASTSpecifierQualifierList>(std::move(*this))
    };
}

std::optional<Node> ASTTypeName::Match(Parser &parser) {
    return Parser::Expect<ASTSpecifierQualifierList>()
            .Builder([](Node &&list) {
                return std::move(list);
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

Node ASTTypeName::ToNode() {
    return Node {
        Node::Type::TypeName,
        std::make_unique<ASTTypeName>(std::move(*this))
    };
}

std::optional<Node> ASTCastExpression::Match(Parser &parser) {
    return Parser::Expect<ASTTypeName>()
            .SurroundedBy(TokenType::LeftParenthesis, TokenType::RightParenthesis)
            .ZeroOrMore()
            .FollowedBy<ASTUnaryExpression>()
            .Builder([](std::vector<Node> &&casts, Node &&unaryExpression) -> Node {
                if (casts.empty()) {
                    return std::move(unaryExpression);
                }

                Node output{std::move(unaryExpression)};

                for (Node &cast : casts) {
                    output = ASTCastExpression{ std::move(output), std::move(cast) }.ToNode();
                }

                return output;
            })
            .Match(parser);
}

std::string ASTCastExpression::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTCastExpression {\n";
    resultStream << tabs << "inner = " << inner.ToString(depth + 1);

    resultStream << ",\n" << tabs << "typeName = " << typeName.ToString(depth + 1);
    resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

Node ASTCastExpression::ToNode() {
    return {
        Node::Type::CastExpression,
        std::make_unique<ASTCastExpression>(std::move(*this))
    };
}

std::string ASTMultiplicativeExpression::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTMultiplicativeExpression {\n";

    resultStream << tabs << "lhs = ";

    if (this->lhs.has_value())
        resultStream << this->lhs->ToString(depth + 1) << ",\n";
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

std::optional<Node> ASTMultiplicativeExpression::Match(Parser &parser) {
    return Parser::Expect<ASTCastExpression>()
            .FollowedBy(
                Parser::Expect(TokenType::Asterisk)
                    .Or(TokenType::Divide)
                    .Or(TokenType::Modulo)
            )
            .ZeroOrMore()
            .FollowedBy<ASTCastExpression>()
            .Builder([](std::vector<std::tuple<Node, Token>> &&multiplicatives, Node &&rhs) -> Node {
                if (multiplicatives.empty())
                    return std::move(rhs);

                std::optional<Node> output{ std::nullopt };
                MultiplicativeOperator op;

                for (auto &multiplicative : multiplicatives) {
                    Node &multLhs{std::get<0>(multiplicative) };
                    Token &opToken{ std::get<1>(multiplicative) };
                    op = ASTMultiplicativeExpression::OperatorFromTokenType(opToken.type);

                    output = ASTMultiplicativeExpression{ std::move(output), op, std::move(multLhs) }.ToNode();
                }

                if (output.has_value()) {
                    Node outputPtr{ std::move(output.value()) };

                    return ASTMultiplicativeExpression{ std::move(outputPtr), op, std::move(rhs) }.ToNode();
                }

                return ASTMultiplicativeExpression{ std::nullopt, MultiplicativeOperator::None, std::move(rhs) }.ToNode();
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

Node ASTMultiplicativeExpression::ToNode() {
    return Node {
        Node::Type::MultiplicativeExpression,
        std::make_unique<ASTMultiplicativeExpression>(std::move(*this))
    };
}

std::string ASTAdditiveExpression::ToString(int depth) const {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << "ASTAdditiveExpression {\n";

    resultStream << tabs << "lhs = ";

    if (this->lhs.has_value())
        resultStream << this->lhs->ToString(depth + 1) << ",\n";
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
std::optional<Node> ASTAdditiveExpression::Match(Parser &parser) {
    return Parser::Expect<ASTMultiplicativeExpression>()
            .FollowedBy(
                    Parser::Expect(TokenType::Plus)
                            .Or(TokenType::Minus)
            )
            .ZeroOrMore()
            .FollowedBy<ASTMultiplicativeExpression>()
            .Builder([](std::vector<std::tuple<Node, Token>> &&additives, Node &&rhs) -> Node {
                if (additives.empty())
                    return std::move(rhs);

                std::optional<Node> output{ std::nullopt };
                AdditiveOperator op;

                for (auto &multiplicative : additives) {
                    Node &multLhs{std::get<0>(multiplicative) };
                    Token &opToken{ std::get<1>(multiplicative) };
                    op = ASTAdditiveExpression::OperatorFromTokenType(opToken.type);

                    output = ASTAdditiveExpression{ std::move(output), op, std::move(multLhs) }.ToNode();
                }

                if (output.has_value()) {
                    Node outputPtr{ std::move(output.value()) };

                    return ASTAdditiveExpression{ std::move(outputPtr), op, std::move(rhs) }.ToNode();
                }

                return ASTAdditiveExpression{ std::nullopt, AdditiveOperator::None, std::move(rhs) }.ToNode();
            })
            .Match(parser);
}

Node ASTAdditiveExpression::ToNode() {
    return Node {
        Node::Type::AdditiveExpression,
        std::make_unique<ASTAdditiveExpression>(std::move(*this))
    };
}

std::string ASTDeclaration::ToString(int depth) const {
    std::stringstream resultStream;

    resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth),     PRETTY_PRINT_CHAR)   << "ASTDeclaration {\n";
    resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "specifier = "  << this->specifier.ToString(depth + 1) << ",\n";
    resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "identifier = " << this->identifier.ToString(depth + 1) << ",\n";
    resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "expression = " << this->expression.ToString(depth + 1);
    resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth),     PRETTY_PRINT_CHAR)   << '}';

    return resultStream.str();
}

std::optional<Node> ASTDeclaration::Match(Parser &parser) {
    return Parser::Expect<ASTSpecifierQualifierList>()
            .FollowedBy<ASTIdentifier>()
            .FollowedByIgnore(TokenType::Assign)
            .FollowedBy<ASTExpression>()
            .FollowedByIgnore(TokenType::Semicolon)
            .Builder([](std::tuple<Node, Node>&& declInfo, Node &&value) -> Node {
                Node &typeSpecifier{ std::get<0>(declInfo) };
                Node &identifier{ std::get<1>(declInfo) };

                return ASTDeclaration{ std::move(typeSpecifier), std::move(identifier), std::move(value) }.ToNode();
            })
            .Match(parser, true);
}

Node ASTDeclaration::ToNode() {
    return Node {
        Node::Type::Declaration,
        std::make_unique<ASTDeclaration>(std::move(*this))
    };
}

std::string Node::ToString(int depth) const {
    return node->ToString(depth);
}
