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

class Parser;

struct ASTNode {

    virtual ~ASTNode() = default;

//    [[nodiscard]]
//    std::unique_ptr<ASTNode> Clone() const;

    [[nodiscard]]
    virtual std::string ToString(int depth) const = 0;

    static constexpr char PRETTY_PRINT_CHAR = '\t';
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
};

struct ASTPrimaryExpression : public ASTNode {
    using Inner = std::variant<ASTIdentifier, ASTConstant>;

    explicit ASTPrimaryExpression(const Inner &identifier) : inner{identifier} {};

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;
        std::string tabs(depth + 1, PRETTY_PRINT_CHAR);

        resultStream << "ASTPrimaryExpression( ";

        if (std::holds_alternative<ASTIdentifier>(this->inner)) {
            resultStream << std::get<ASTIdentifier>(this->inner).ToString(depth + 1);
        } else {
            resultStream << std::get<ASTConstant>(this->inner).ToString(depth + 1);
        }

        resultStream << " )";

        return resultStream.str();
    }

    std::variant<ASTIdentifier, ASTConstant> inner;
};

struct ASTPostfixIncrement final : public ASTNode {
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTPostfixIncrement";
    }
};

struct ASTPostfixDecrement final : public ASTNode {
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTPostfixDecrement";
    }
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
};

struct ASTPostfixExpression final : public ASTNode {
    using Operation = std::variant<ASTPostfixIncrement, ASTPostfixDecrement, ASTPostfixArrow, ASTPostfixDot>;
    using OptionalOperation = std::optional<Operation>;

    ASTPostfixExpression(ASTPostfixExpression &&other, const OptionalOperation &operation)
        : inner{
            std::make_unique<ASTPostfixExpression>(std::move(other))
        }
        , operation{operation} {};

    ASTPostfixExpression(const ASTPrimaryExpression &primaryExpression, const OptionalOperation &operation)
        : inner{
            primaryExpression
        }
        , operation{operation} {};

    ASTPostfixExpression(const ASTPostfixExpression &expression) {
        if (std::holds_alternative<std::unique_ptr<ASTPostfixExpression>>(expression.inner)) {
            this->inner = std::make_unique<ASTPostfixExpression>(*std::get<std::unique_ptr<ASTPostfixExpression>>(expression.inner));
        } else {
            this->inner = std::get<ASTPrimaryExpression>(expression.inner);
        }

        this->operation = expression.operation;
    }

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

        std::string tabs(depth + 1, PRETTY_PRINT_CHAR);

        resultStream << "ASTPostfixExpression {\n";
        resultStream << tabs << "inner = ";

        if (std::holds_alternative<std::unique_ptr<ASTPostfixExpression>>(inner)) {
            resultStream << std::get<std::unique_ptr<ASTPostfixExpression>>(inner)->ToString(depth + 1);
        } else {
            resultStream << std::get<ASTPrimaryExpression>(inner).ToString(depth + 1);
        }

        if (operation.has_value()) {
            resultStream << ",\n" << tabs << "operation = ";

            resultStream << std::visit([](const auto &op) {
                return op.ToString(0);
            }, operation.value());
        }
        resultStream << '\n' << std::string(depth, PRETTY_PRINT_CHAR) << '}';

        return resultStream.str();
    }

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
public:
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTUnaryIncrement";
    }
};

struct ASTUnaryDecrement final : public ASTNode {
public:
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTUnaryDecrement";
    }
};

struct ASTUnaryExpression final : public ASTNode {
public:
    using Operator = std::variant<ASTUnaryOperator, ASTUnaryIncrement, ASTUnaryDecrement>;

    ASTUnaryExpression(ASTUnaryExpression &&other, const Operator &operation)
        : inner{
            std::make_unique<ASTUnaryExpression>(std::move(other))
        }
        , operation{operation} {};


    ASTUnaryExpression(const ASTUnaryExpression &other)
        : operation{other.operation} {
        if (std::holds_alternative<std::unique_ptr<ASTUnaryExpression>>(other.inner)) {
            this->inner = std::make_unique<ASTUnaryExpression>(*std::get<std::unique_ptr<ASTUnaryExpression>>(other.inner));
        } else {
            this->inner = std::get<ASTPostfixExpression>(other.inner);
        }
    };

    ASTUnaryExpression(const ASTPostfixExpression &astPostfixExpression, const std::optional<Operator> &operation)
        : inner{
            astPostfixExpression
        }
        , operation{operation} {};

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
    std::string ToString(int depth) const override {
        std::stringstream resultStream{};

        std::string tabs(depth + 1, PRETTY_PRINT_CHAR);

        resultStream << "ASTUnaryExpression {\n";
        resultStream << tabs << "inner = ";

        if (std::holds_alternative<std::unique_ptr<ASTUnaryExpression>>(inner)) {
            resultStream << std::get<std::unique_ptr<ASTUnaryExpression>>(inner)->ToString(depth + 1);
        } else {
            resultStream << std::get<ASTPostfixExpression>(inner).ToString(depth + 1);
        }

        if (operation.has_value()) {
            resultStream << ",\n" << tabs << "operation = ";

            resultStream << std::visit([](const auto &op) {
                return op.ToString(0);
            }, operation.value());
        }
        resultStream << '\n' << std::string(depth, PRETTY_PRINT_CHAR) << '}';

        return resultStream.str();
    }

private:
    std::variant<std::unique_ptr<ASTUnaryExpression>, ASTPostfixExpression> inner;
    std::optional<Operator> operation;
};

struct ASTExpression : public ASTNode {
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

struct ASTTypeQualifier : public ASTNode {
    enum class TypeQualifier {
        Const,
        Restrict,
        Volatile,
        Atomic,
    } qualifier{};

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
};

struct ASTDeclaration final : public ASTNode {

    explicit ASTDeclaration(const ASTTypeSpecifier &specifier, const ASTIdentifier &identifier, const ASTUnaryExpression &expression) : identifier{identifier}, specifier{specifier},
                                                                                                                                      expression{expression} {};
    
    ASTTypeSpecifier specifier;
    ASTIdentifier identifier;
    ASTUnaryExpression expression;

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;

        resultStream <<         std::string(depth,     PRETTY_PRINT_CHAR)   << "ASTDeclaration {\n";
        resultStream <<         std::string(depth + 1, PRETTY_PRINT_CHAR)   << "specifier  = " << this->specifier.ToString(depth + 1) << ",\n";
        resultStream <<         std::string(depth + 1, PRETTY_PRINT_CHAR)   << "identifier = " << this->identifier.ToString(depth + 1) << ",\n";
        resultStream <<         std::string(depth + 1, PRETTY_PRINT_CHAR)   << "expression = " << this->expression.ToString(depth + 1);
        resultStream << '\n' << std::string(depth,     PRETTY_PRINT_CHAR)   << '}';

        return resultStream.str();
    }
};

#endif //JCC_ASTNODE_H
