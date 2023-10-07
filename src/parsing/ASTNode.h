//
// Created by johny on 7/21/23.
//

#ifndef JCC_ASTNODE_H
#define JCC_ASTNODE_H


#include <variant>
#include <vector>
#include <memory>
#include <optional>
#include <sstream>
#include "../Span.h"
#include "../tokenizer.h"
#include "../../libs/magic_enum/magic_enum.hpp"

// todo: we should use moves for the ast node constructors

#define PRETTY_PRINT_CHAR ' '
#define PRETTY_PRINT_FACTOR 2
#define PRETTY_PRINT_DEPTH(depth) ((depth) * PRETTY_PRINT_FACTOR)

class Parser;

struct ASTNode {

    virtual ~ASTNode() = default;

//    [[nodiscard]]
//    std::unique_ptr<ASTNode> Clone() const;

    [[nodiscard]]
    virtual std::string ToString(int depth) const = 0;
};

struct ASTIdentifier : public ASTNode {
    std::string identifier;

    explicit ASTIdentifier(const std::string &identifier) : identifier{identifier} {};

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;

        resultStream << "ASTIdentifier(\"" << this->identifier << "\")";

        return resultStream.str();
    }

    static std::optional<ASTIdentifier> Match(Parser &parser);
};

struct ASTConstant final : public ASTNode {
    explicit ASTConstant(const Token &token) : constantToken{ token } {}

    Token constantToken;

    [[nodiscard]]
    std::string ToString(int depth) const override {
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

    static std::optional<ASTConstant> Match(Parser &parser);
};

struct ASTExpression;

struct ASTPrimaryExpression : public ASTNode {
    using Inner = std::variant<ASTIdentifier, ASTConstant, std::unique_ptr<ASTExpression>>;

    explicit ASTPrimaryExpression(Inner &&identifier) : inner{std::move(identifier)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override;

    static std::optional<ASTPrimaryExpression> Match(Parser &parser);

    Inner inner;
};

struct ASTPostfixIncrement final : public ASTNode {
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTPostfixIncrement";
    }

    static std::optional<ASTPostfixIncrement> Match(Parser &parser);
};

struct ASTPostfixDecrement final : public ASTNode {
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTPostfixDecrement";
    }

    static std::optional<ASTPostfixDecrement> Match(Parser &parser);
};

struct ASTPostfixArrow final : public ASTNode {
    explicit ASTPostfixArrow(const ASTIdentifier &identifier) : identifier{identifier} {};

    ASTIdentifier identifier;

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;

        resultStream << "ASTPostfixArrow( " << this->identifier.ToString(depth + 1) << " )";

        return resultStream.str();
    }

    static std::optional<ASTPostfixArrow> Match(Parser &parser);
};

struct ASTExpression;

struct ASTPostfixIndex final : public ASTNode {
    explicit ASTPostfixIndex(std::unique_ptr<ASTExpression> &&innerExpression) : inner{std::move(innerExpression)} {};

    std::unique_ptr<ASTExpression> inner;

    [[nodiscard]]
    std::string ToString(int depth) const override;

    static std::optional<ASTPostfixIndex> Match(Parser &parser);
};

struct ASTPostfixDot final : public ASTNode {
    explicit ASTPostfixDot(const ASTIdentifier &identifier) : identifier{identifier} {};

    ASTIdentifier identifier;

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;

        resultStream << "ASTPostfixDot( " << this->identifier.ToString(depth + 1) << " )";

        return resultStream.str();
    }

    static std::optional<ASTPostfixDot> Match(Parser &parser);
};

struct ASTPostfixExpression final : public ASTNode {
    using Operation = std::variant<ASTPostfixIncrement, ASTPostfixDecrement, ASTPostfixArrow, ASTPostfixDot, ASTPostfixIndex>;
    using OptionalOperation = std::optional<Operation>;

    ASTPostfixExpression(ASTPostfixExpression &&other, OptionalOperation &&operation)
        : inner{
            std::make_unique<ASTPostfixExpression>(std::move(other))
        }
        , operation{std::move(operation)} {};

    ASTPostfixExpression(ASTPrimaryExpression &&primaryExpression, OptionalOperation &&operation)
        : inner{
            std::move(primaryExpression)
        }
        , operation{std::move(operation)} {};

    ASTPostfixExpression(ASTPostfixExpression &&expression) noexcept {
        if (std::holds_alternative<std::unique_ptr<ASTPostfixExpression>>(expression.inner)) {
            this->inner = std::move(std::get<std::unique_ptr<ASTPostfixExpression>>(expression.inner));
        } else {
            this->inner = std::move(std::get<ASTPrimaryExpression>(expression.inner));
        }

        this->operation = std::move(expression.operation);
    }

    ASTPostfixExpression &operator=(ASTPostfixExpression &&expression) noexcept {
        if (std::holds_alternative<std::unique_ptr<ASTPostfixExpression>>(expression.inner)) {
            this->inner = std::move(std::get<std::unique_ptr<ASTPostfixExpression>>(expression.inner));
        } else {
            this->inner = std::move(std::get<ASTPrimaryExpression>(expression.inner));
        }

        this->operation = std::move(expression.operation);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream{};

        std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

        resultStream << "ASTPostfixExpression {\n";
        resultStream << tabs << "inner = ";

        if (std::holds_alternative<std::unique_ptr<ASTPostfixExpression>>(inner)) {
            resultStream << std::get<std::unique_ptr<ASTPostfixExpression>>(inner)->ToString(depth + 1);
        } else {
            resultStream << std::get<ASTPrimaryExpression>(inner).ToString(depth + 1);
        }

        if (operation.has_value()) {
            resultStream << ",\n" << tabs << "operation = ";

            resultStream << std::visit([&depth](const auto &op) {
                return op.ToString(depth + 1);
            }, operation.value());
        }
        resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

        return resultStream.str();
    }

    static std::optional<ASTPostfixExpression> Match(Parser &parser);

    std::variant<std::unique_ptr<ASTPostfixExpression>, ASTPrimaryExpression> inner;
    OptionalOperation operation;
};

struct ASTUnaryOperator final : public ASTNode {
public:
    explicit ASTUnaryOperator(TokenType tokenType) {
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

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;

        resultStream << "ASTUnaryOperator(" << magic_enum::enum_name(this->unaryOperator) << ")";

        return resultStream.str();
    }

    static std::optional<ASTUnaryOperator> Match(Parser &parser);

private:
    enum class UnaryOperator {
        Reference,
        Dereference,
        Positive,
        Negative,
        BitwiseNot,
        Not
    } unaryOperator;
};

struct ASTUnaryIncrement final : public ASTNode {
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTUnaryIncrement";
    }

    static std::optional<ASTUnaryIncrement> Match(Parser &parser);
};

struct ASTUnaryDecrement final : public ASTNode {
public:
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTUnaryDecrement";
    }

    static std::optional<ASTUnaryDecrement> Match(Parser &parser);
};

struct ASTCastExpression;

struct ASTUnaryExpression final : public ASTNode {
public:
    using Operator = std::variant<ASTUnaryOperator, ASTUnaryIncrement, ASTUnaryDecrement>;

    ASTUnaryExpression(ASTUnaryExpression &&other, Operator &&operation)
        : inner{
            std::make_unique<ASTUnaryExpression>(std::move(other))
        }
        , operation{std::move(operation)} {}


    ASTUnaryExpression(ASTUnaryExpression &&other) noexcept
        : operation{std::move(other.operation)}
        , inner{std::move(other.inner)} {
    }

    ASTUnaryExpression(ASTPostfixExpression &&astPostfixExpression, std::optional<Operator> &&operation)
        : inner{
            std::move(astPostfixExpression)
        }
        , operation{std::move(operation)} {}

    ASTUnaryExpression(std::unique_ptr<ASTCastExpression> &&castExpression, std::optional<Operator> &&operation)
        : inner{
            std::move(castExpression)
        }
        , operation{std::move(operation)} {}

    ASTUnaryExpression &operator=(ASTUnaryExpression &&other) noexcept {
        if (std::holds_alternative<std::unique_ptr<ASTUnaryExpression>>(other.inner)) {
            this->inner = std::move(std::get<std::unique_ptr<ASTUnaryExpression>>(other.inner));
        } else {
            this->inner = std::move(std::get<ASTPostfixExpression>(other.inner));
        }

        this->operation = std::move(other.operation);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    static std::optional<ASTUnaryExpression> Match(Parser &parser);

private:
    std::variant<std::unique_ptr<ASTUnaryExpression>, ASTPostfixExpression, std::unique_ptr<ASTCastExpression>> inner;
    std::optional<Operator> operation;
};

struct ASTTypeQualifier : public ASTNode {
    enum class TypeQualifier {
        Const,
        Restrict,
        Volatile,
        Atomic,
    } qualifier{};

    explicit ASTTypeQualifier(TypeQualifier qualifier) : qualifier{qualifier} {};

    static TypeQualifier FromTokenType(TokenType tokenType) {
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

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream{};

        resultStream << "ASTTypeQualifier(" << magic_enum::enum_name(this->qualifier) << ')';

        return resultStream.str();
    }

    static std::optional<ASTTypeQualifier> Match(Parser &parser);
};

struct ASTTypeSpecifier final : public ASTNode {
    enum class TypeSpecifier {
        Void,
        Char,
        Short,
        Int,
        Long,
        Float,
        Double,
        Signed,
        Unsigned,
        Bool,
        Complex,
    } specifier{};

    explicit ASTTypeSpecifier(TypeSpecifier specifier) : specifier{specifier} {};

    static TypeSpecifier FromTokenType(TokenType tokenType) {
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

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream{};

        resultStream << "ASTTypeSpecifier(" << magic_enum::enum_name(this->specifier) << ')';

        return resultStream.str();
    }

    static std::optional<ASTTypeSpecifier> Match(Parser &parser);
};

struct ASTSpecifierQualifierList final : public ASTNode {
    explicit ASTSpecifierQualifierList(std::vector<std::variant<ASTTypeSpecifier, ASTTypeQualifier>> &&list)
            : list{std::move(list)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override {
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

    static std::optional<ASTSpecifierQualifierList> Match(Parser &parser);

    std::vector<std::variant<ASTTypeSpecifier, ASTTypeQualifier>> list;
};

struct ASTTypeName final : public ASTNode {
    explicit ASTTypeName(ASTSpecifierQualifierList &&specifierQualifierList)
            : specifierQualifierList{std::move(specifierQualifierList)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream{};

        std::string tabs(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR);

        resultStream << "ASTTypeName {\n";
        resultStream << tabs << "specifierQualifierList = " << this->specifierQualifierList.ToString(depth + 1) << '\n';
        resultStream << std::string(PRETTY_PRINT_DEPTH(depth), PRETTY_PRINT_CHAR) << '}';

        return resultStream.str();
    }

    static std::optional<ASTTypeName> Match(Parser &list);

    ASTSpecifierQualifierList specifierQualifierList;
};

struct ASTCastExpression final : public ASTNode {
    ASTCastExpression(ASTUnaryExpression &&unaryExpression, std::optional<ASTTypeName> &&typeName)
            : inner{std::move(unaryExpression)}
            , typeName{std::move(typeName)} {};

    ASTCastExpression(std::unique_ptr<ASTCastExpression> &&castExpressionPtr, std::optional<ASTTypeName> &&typeName)
            : inner{std::move(castExpressionPtr)}
            , typeName{std::move(typeName)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override {
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

    static std::optional<ASTCastExpression> Match(Parser &parser);

    std::variant<ASTUnaryExpression, std::unique_ptr<ASTCastExpression>> inner;
    std::optional<ASTTypeName> typeName;
};

struct ASTExpression final : public ASTNode {
    ASTExpression(ASTCastExpression &&first, std::vector<ASTCastExpression> &&other)
        : first{std::move(first)}
        , expressions{std::move(other)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override {
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

    static std::optional<ASTExpression> Match(Parser &parser);

    ASTCastExpression first;
    std::vector<ASTCastExpression> expressions;
};

struct ASTDeclarator : public ASTNode {
};

struct ASTInitDeclarator : public ASTNode {
};

struct ASTDeclaratorList : public ASTNode {
};

struct ASTFunctionSpecifier : public ASTNode {
    enum class Specifier {
        Inline,
        NoReturn,
    } specifier{};

};

struct ASTDeclaration final : public ASTNode {

    explicit ASTDeclaration(
            ASTSpecifierQualifierList &&list,
            ASTIdentifier    &&identifier,
            ASTExpression    &&expression
    )
    : identifier{std::move(identifier)}
    , specifier {std::move(list)}
    , expression{std::move(expression)} {};

    ASTSpecifierQualifierList specifier;
    ASTIdentifier identifier;
    ASTExpression expression;

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;

        resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth),     PRETTY_PRINT_CHAR)   << "ASTDeclaration {\n";
        resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "specifier  = " << this->specifier.ToString(depth + 1) << ",\n";
        resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "identifier = " << this->identifier.ToString(depth + 1) << ",\n";
        resultStream <<         std::string(PRETTY_PRINT_DEPTH(depth + 1), PRETTY_PRINT_CHAR)   << "expression = " << this->expression.ToString(depth + 1);
        resultStream << '\n' << std::string(PRETTY_PRINT_DEPTH(depth),     PRETTY_PRINT_CHAR)   << '}';

        return resultStream.str();
    }
};

#endif //JCC_ASTNODE_H
