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

std::string ASTPostfixDot::ToString(int depth) const {
    std::stringstream resultStream;

    resultStream << "ASTPostfixDot( " << this->identifier.ToString(depth + 1) << " )";

    return resultStream.str();
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
    return Parser::Expect<ASTLogicalOrExpression>()
            .FollowedBy(
            Parser::Expect(TokenType::Comma)
                        .FollowedBy<ASTLogicalOrExpression>()
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

ASTTypeQualifier::TypeQualifier ASTTypeQualifier::FromTokenType(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::KeywordConst:
            return ASTTypeQualifier::TypeQualifier::Const;
        case TokenType::KeywordRestrict:
            return ASTTypeQualifier::TypeQualifier::Restrict;
        case TokenType::KeywordVolatile:
            return ASTTypeQualifier::TypeQualifier::Volatile;
        case TokenType::KeywordAtomic:
            return ASTTypeQualifier::TypeQualifier::Atomic;
        default:
            assert(false);
    }
}

std::string ASTTypeQualifier::ToString(int depth) const {
    std::stringstream resultStream{};

    resultStream << "ASTTypeQualifier(" << magic_enum::enum_name(this->qualifier) << ')';

    return resultStream.str();
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

ASTTypeSpecifier::TypeSpecifier ASTTypeSpecifier::FromTokenType(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::KeywordVoid:
            return ASTTypeSpecifier::TypeSpecifier::Void;
        case TokenType::KeywordChar:
            return ASTTypeSpecifier::TypeSpecifier::Char;
        case TokenType::KeywordShort:
            return ASTTypeSpecifier::TypeSpecifier::Short;
        case TokenType::KeywordInt:
            return ASTTypeSpecifier::TypeSpecifier::Int;
        case TokenType::KeywordLong:
            return ASTTypeSpecifier::TypeSpecifier::Long;
        case TokenType::KeywordFloat:
            return ASTTypeSpecifier::TypeSpecifier::Float;
        case TokenType::KeywordDouble:
            return ASTTypeSpecifier::TypeSpecifier::Double;
        case TokenType::KeywordSigned:
            return ASTTypeSpecifier::TypeSpecifier::Signed;
        case TokenType::KeywordUnsigned:
            return ASTTypeSpecifier::TypeSpecifier::Unsigned;
        case TokenType::KeywordBool:
            return ASTTypeSpecifier::TypeSpecifier::Bool;
        case TokenType::KeywordComplex:
            return ASTTypeSpecifier::TypeSpecifier::Complex;
        default:
            assert(false);
    }
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

template<class TNode>
std::string BinaryExpressionToString(const TNode &node, std::string &&nodeName, int depth) {
    std::stringstream resultStream{};

    std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

    resultStream << nodeName << " {\n";

    resultStream << tabs << "lhs = ";

    if (node.lhs.has_value())
        resultStream << node.lhs->ToString(depth + 1) << ",\n";
    else
        resultStream << "none,\n";

    resultStream << tabs << "rhs = ";

    if (node.rhs.has_value())
        resultStream << node.rhs->ToString(depth + 1) << ",\n";
    else
        resultStream << "none,\n";

    resultStream << tabs << "operator = " << magic_enum::enum_name(node.operatorType) << '\n';

    resultStream << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

    return resultStream.str();
}

template<class TChildren, class TOut, class TTokenRule>
inline std::optional<Node> BinaryExpression(const TTokenRule &tokenRule, Parser &parser) {
    return Parser::Expect<TChildren>()
            .FollowedBy(tokenRule)
            .ZeroOrMore()
            .template FollowedBy<TChildren>()
            .Builder([](std::vector<std::tuple<Node, Token>> &&expressions, Node &&rhs) -> Node {
                if (expressions.empty())
                    return std::move(rhs);

                std::optional<Node> output{ std::nullopt };
                typename TOut::Operator op;

                for (auto &expression : expressions) {
                    Node &lhs{std::get<0>(expression) };
                    Token &opToken{ std::get<1>(expression) };
                    op = TOut::OperatorFromTokenType(opToken.type);

                    if (!output.has_value()) {
                        output = std::move(lhs);
                        continue;
                    }

                    output = TOut{ std::move(output), op, std::move(lhs) }.ToNode();
                }

                if (output.has_value()) {
                    Node outputPtr{ std::move(output.value()) };

                    return TOut{ std::move(outputPtr), op, std::move(rhs) }.ToNode();
                }

                return TOut{ std::nullopt, TOut::Operator::None, std::move(rhs) }.ToNode();
            })
            .Match(parser);
}

std::optional<Node> ASTMultiplicativeExpression::Match(Parser &parser) {
    auto tokenRule{
        Parser::Expect(TokenType::Asterisk)
           .Or(TokenType::Divide)
           .Or(TokenType::Modulo)
    };

    return BinaryExpression<ASTCastExpression, ASTMultiplicativeExpression, typeof tokenRule>(tokenRule, parser);
}

ASTMultiplicativeExpression::Operator
ASTMultiplicativeExpression::OperatorFromTokenType(TokenType tokenType) {
    {
        switch (tokenType) {
            case TokenType::Asterisk:
                return Operator::Multiply;
            case TokenType::Divide:
                return Operator::Divide;
            case TokenType::Modulo:
                return Operator::Modulo;
            default:
                assert(false && "invalid token type");
                return Operator::None;
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
    return BinaryExpressionToString(*this, "ASTAdditiveExpression", depth);
}

ASTAdditiveExpression::Operator
ASTAdditiveExpression::OperatorFromTokenType(TokenType tokenType) {
    {
        switch (tokenType) {
            case TokenType::Plus:
                return Operator::Add;
            case TokenType::Minus:
                return Operator::Subtract;
            default:
                assert(false && "invalid token type");
                return Operator::None;
        }
    }
}

std::optional<Node> ASTAdditiveExpression::Match(Parser &parser) {
    auto tokenRule{
        Parser::Expect(TokenType::Plus)
            .Or(TokenType::Minus)
    };

    return BinaryExpression<ASTMultiplicativeExpression, ASTAdditiveExpression, typeof tokenRule>(tokenRule, parser);
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

std::string ASTShiftExpression::ToString(int depth) const {
    return BinaryExpressionToString(*this, "ASTShiftExpression", depth);
}

Node ASTShiftExpression::ToNode() {
    return Node {
        Node::Type::ShiftExpression,
        std::make_unique<ASTShiftExpression>(std::move(*this))
    };
}

std::optional<Node> ASTShiftExpression::Match(Parser &parser) {
    auto tokenRule{
        Parser::Expect(TokenType::ShiftLeft)
            .Or(TokenType::ShiftRight)
    };

    return BinaryExpression<ASTAdditiveExpression, ASTShiftExpression, typeof tokenRule>(tokenRule, parser);
}

ASTShiftExpression::Operator ASTShiftExpression::OperatorFromTokenType(TokenType tokenType) {
    {
        switch (tokenType) {
            case TokenType::ShiftLeft:
                return Operator::Left;
            case TokenType::ShiftRight:
                return Operator::Right;
            default:
                assert(false && "invalid token type");
                return Operator::None;
        }
    }
}

std::string ASTRelationalExpression::ToString(int depth) const {
    return BinaryExpressionToString(*this, "ASTRelationalExpression", depth);
}

Node ASTRelationalExpression::ToNode() {
    return Node {
        Node::Type::RelationalExpression,
        std::make_unique<ASTRelationalExpression>(std::move(*this))
    };
}

std::optional<Node> ASTRelationalExpression::Match(Parser &parser) {
    auto tokenRule{
        Parser::Expect(TokenType::LessThan)
            .Or(TokenType::GreaterThan)
            .Or(TokenType::LessThanOrEqual)
            .Or(TokenType::GreaterThanOrEqual)
    };

    return BinaryExpression<ASTShiftExpression, ASTRelationalExpression, typeof tokenRule>(tokenRule, parser);
}

ASTRelationalExpression::Operator ASTRelationalExpression::OperatorFromTokenType(TokenType tokenType) {
    {
        switch (tokenType) {
            case TokenType::LessThan:
                return Operator::LessThan;
            case TokenType::GreaterThan:
                return Operator::GreaterThan;
            case TokenType::LessThanOrEqual:
                return Operator::LessThanOrEqual;
            case TokenType::GreaterThanOrEqual:
                return Operator::GreaterThanOrEqual;
            default:
                assert(false && "invalid token type");
                return Operator::None;
        }
    }
}

std::string ASTEqualityExpression::ToString(int depth) const {
    return BinaryExpressionToString(*this, "ASTEqualityExpression", depth);
}

std::optional<Node> ASTEqualityExpression::Match(Parser &equality) {
    auto tokenRule{
        Parser::Expect(TokenType::Equals)
            .Or(TokenType::NotEquals)
    };

    return BinaryExpression<ASTRelationalExpression, ASTEqualityExpression, typeof tokenRule>(tokenRule, equality);
}

Node ASTEqualityExpression::ToNode() {
    return Node {
        Node::Type::EqualityExpression,
        std::make_unique<ASTEqualityExpression>(std::move(*this))
    };
}

ASTEqualityExpression::Operator ASTEqualityExpression::OperatorFromTokenType(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::Equals:
            return Operator::Equal;
        case TokenType::NotEquals:
            return Operator::NotEqual;
        default:
            assert(false && "invalid token type");
            return Operator::None;
    }
}

std::string ASTAndExpression::ToString(int depth) const {
    return BinaryExpressionToString(*this, "ASTAndExpression", depth);
}

Node ASTAndExpression::ToNode() {
    return Node {
        Node::Type::AndExpression,
        std::make_unique<ASTAndExpression>(std::move(*this))
    };
}

std::optional<Node> ASTAndExpression::Match(Parser &parser) {
    auto tokenRule{ Parser::Expect(TokenType::Ampersand) };

    return BinaryExpression<ASTEqualityExpression, ASTAndExpression, typeof tokenRule>(tokenRule, parser);;
}

ASTAndExpression::Operator ASTAndExpression::OperatorFromTokenType(TokenType tokenType) {
    return Operator::And;
}

std::string ASTExclusiveOrExpression::ToString(int depth) const {
    return BinaryExpressionToString(*this, "ASTExclusiveOrExpression", depth);
}

Node ASTExclusiveOrExpression::ToNode() {
    return Node {
        Node::Type::ExclusiveOrExpression,
        std::make_unique<ASTExclusiveOrExpression>(std::move(*this))
    };
}

std::optional<Node> ASTExclusiveOrExpression::Match(Parser &parser) {
    auto tokenRule{ Parser::Expect(TokenType::ExclusiveOr) };

    return BinaryExpression<ASTAndExpression, ASTExclusiveOrExpression, typeof tokenRule>(tokenRule, parser);
}

ASTExclusiveOrExpression::Operator ASTExclusiveOrExpression::OperatorFromTokenType(TokenType tokenType) {
    return Operator::ExclusiveOr;
}

std::string ASTInclusiveOrExpression::ToString(int depth) const {
    return BinaryExpressionToString(*this, "ASTInclusiveOrExpression", depth);
}

Node ASTInclusiveOrExpression::ToNode() {
    return Node {
        Node::Type::InclusiveOrExpression,
        std::make_unique<ASTInclusiveOrExpression>(std::move(*this))
    };
}

std::optional<Node> ASTInclusiveOrExpression::Match(Parser &parser) {
    auto tokenRule{ Parser::Expect(TokenType::BitwiseOr) };

    return BinaryExpression<ASTExclusiveOrExpression, ASTInclusiveOrExpression, typeof tokenRule>(tokenRule, parser);
}

ASTInclusiveOrExpression::Operator ASTInclusiveOrExpression::OperatorFromTokenType(TokenType tokenType) {
    return Operator::InclusiveOr;
}

std::string ASTLogicalAndExpression::ToString(int depth) const {
    return BinaryExpressionToString(*this, "ASTLogicalAndExpression", depth);
}

Node ASTLogicalAndExpression::ToNode() {
    return Node {
        Node::Type::LogicalAndExpression,
        std::make_unique<ASTLogicalAndExpression>(std::move(*this))
    };
}

std::optional<Node> ASTLogicalAndExpression::Match(Parser &parser) {
    auto tokenRule{ Parser::Expect(TokenType::LogicalAnd) };

    return BinaryExpression<ASTInclusiveOrExpression, ASTLogicalAndExpression, typeof tokenRule>(tokenRule, parser);
}

ASTLogicalAndExpression::Operator ASTLogicalAndExpression::OperatorFromTokenType(TokenType tokenType) {
    return Operator::LogicalAnd;
}

std::string ASTLogicalOrExpression::ToString(int depth) const {
    return BinaryExpressionToString(*this, "ASTLogicalOrExpression", depth);
}

Node ASTLogicalOrExpression::ToNode() {
    return Node {
        Node::Type::LogicalOrExpression,
        std::make_unique<ASTLogicalOrExpression>(std::move(*this))
    };
}

std::optional<Node> ASTLogicalOrExpression::Match(Parser &parser) {
    auto tokenRule{ Parser::Expect(TokenType::LogicalOr) };

    return BinaryExpression<ASTLogicalAndExpression, ASTLogicalOrExpression, typeof tokenRule>(tokenRule, parser);
}

ASTLogicalOrExpression::Operator ASTLogicalOrExpression::OperatorFromTokenType(TokenType tokenType) {
    return Operator::LogicalOr;
}