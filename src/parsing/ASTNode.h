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

struct Node;

struct ASTNode {

    virtual ~ASTNode() = default;

//    [[nodiscard]]
//    std::unique_ptr<ASTNode> Clone() const;

    [[nodiscard]]
    virtual std::string ToString(int depth) const = 0;

    [[nodiscard]]
    virtual Node ToNode() = 0;
};

struct Node final {
    enum class Type {
        Identifier,
        Constant,
        PrimaryExpression,
        PostfixIncrement,
        PostfixDecrement,
        PostfixArrow,
        PostfixIndex,
        PostfixDot,
        PostfixExpression,
        UnaryOperator,
        UnaryIncrement,
        UnaryDecrement,
        UnaryExpression,
        CastExpression,
        TypeQualifier,
        TypeSpecifier,
        SpecifierQualifierList,
        TypeName,
        MultiplicativeExpression,
        AdditiveExpression,
        Declaration,
        ShiftExpression,
        RelationalExpression,
        EqualityExpression,
        AndExpression,
        ExclusiveOrExpression,
        InclusiveOrExpression,
        LogicalAndExpression,
        LogicalOrExpression,
        ConditionalExpression,
        AssignmentExpression,
        AssignmentOperator,
        Expression,
    } type;

    std::unique_ptr<ASTNode> node;

    [[nodiscard]]
    std::string ToString(int depth) const;
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

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTConstant final : public ASTNode {
    explicit ASTConstant(const Token &token) : constantToken{ token } {}

    Token constantToken;

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTExpression;

struct ASTPrimaryExpression final : public ASTNode {
    using Inner = std::variant<ASTIdentifier, ASTConstant, std::unique_ptr<ASTExpression>>;

    explicit ASTPrimaryExpression(Inner &&identifier) : inner{std::move(identifier)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    Inner inner;
};

struct ASTPostfixIncrement final : public ASTNode {
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTPostfixIncrement";
    }

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTPostfixDecrement final : public ASTNode {
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTPostfixDecrement";
    }

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTPostfixArrow final : public ASTNode {
    explicit ASTPostfixArrow(Node &&identifier) : identifier{std::move(identifier)} {};

    Node identifier;

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;

        resultStream << "ASTPostfixArrow( " << this->identifier.ToString(depth + 1) << " )";

        return resultStream.str();
    }

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTExpression;

struct ASTPostfixIndex final : public ASTNode {
    explicit ASTPostfixIndex(Node &&innerExpression) : inner{std::move(innerExpression)} {};

    Node inner;

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTPostfixDot final : public ASTNode {
    explicit ASTPostfixDot(Node &&identifier) : identifier{std::move(identifier)} {};

    Node identifier;

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTPostfixExpression final : public ASTNode {
    ASTPostfixExpression(Node &&other, Node &&operation)
        : inner{std::move(other)}
        , operation{std::move(operation)} {}

    ASTPostfixExpression(ASTPostfixExpression &&expression) noexcept
        : inner{std::move(expression.inner)}
        , operation{std::move(expression.operation)} {}

    ASTPostfixExpression &operator=(ASTPostfixExpression &&expression) noexcept {
        this->inner = std::move(expression.inner);

        this->operation = std::move(expression.operation);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    Node inner;
    Node operation;
};

struct ASTUnaryOperator final : public ASTNode {
public:
    explicit ASTUnaryOperator(TokenType tokenType);

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

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

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTUnaryDecrement final : public ASTNode {
public:
    [[nodiscard]]
    std::string ToString(int depth) const override {
        return "ASTUnaryDecrement";
    }

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTCastExpression;

struct ASTUnaryExpression final : public ASTNode {
public:
    ASTUnaryExpression(Node &&other, Node &&operation)
        : inner{std::move(other)}
        , operation{std::move(operation)} {}


    ASTUnaryExpression(ASTUnaryExpression &&other) noexcept
        : operation{std::move(other.operation)}
        , inner{std::move(other.inner)} {
    }

    ASTUnaryExpression &operator=(ASTUnaryExpression &&other) noexcept {
        this->inner = std::move(other.inner);
        this->operation = std::move(other.operation);

        return *this;
    }

    Node ToNode() override;

    [[nodiscard]]
    std::string ToString(int depth) const override;

    static std::optional<Node> Match(Parser &parser);

private:
    Node inner;
    Node operation;
};

struct ASTTypeQualifier : public ASTNode {
    enum class TypeQualifier {
        Const,
        Restrict,
        Volatile,
        Atomic,
    } qualifier{};

    explicit ASTTypeQualifier(TypeQualifier qualifier) : qualifier{qualifier} {};

    static TypeQualifier FromTokenType(TokenType tokenType);

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
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

    static TypeSpecifier FromTokenType(TokenType tokenType);

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

struct ASTSpecifierQualifierList final : public ASTNode {
    explicit ASTSpecifierQualifierList(std::vector<Node> &&list)
            : list{std::move(list)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    std::vector<Node> list;
};

struct ASTTypeName final : public ASTNode {
    explicit ASTTypeName(ASTSpecifierQualifierList &&specifierQualifierList)
            : specifierQualifierList{std::move(specifierQualifierList)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &list);

    ASTSpecifierQualifierList specifierQualifierList;
};

struct ASTCastExpression final : public ASTNode {
    ASTCastExpression(Node &&unaryExpression, Node &&typeName)
            : inner{std::move(unaryExpression)}
            , typeName{std::move(typeName)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    Node inner;
    Node typeName;
};

struct ASTMultiplicativeExpression final : public ASTNode {
    enum class Operator {
        Multiply,
        Divide,
        Modulo,
        None
    };

    ASTMultiplicativeExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTMultiplicativeExpression(ASTMultiplicativeExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTMultiplicativeExpression &operator=(ASTMultiplicativeExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTAdditiveExpression final : public ASTNode {
    enum class Operator {
        Add,
        Subtract,
        None,
    };

    ASTAdditiveExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTAdditiveExpression(ASTAdditiveExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTAdditiveExpression &operator=(ASTAdditiveExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTShiftExpression final : public ASTNode {
    enum class Operator {
        Left,
        Right,
        None,
    };

    ASTShiftExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTShiftExpression(ASTShiftExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTShiftExpression &operator=(ASTShiftExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTRelationalExpression final : public ASTNode {
    enum class Operator {
        LessThan,
        GreaterThan,
        LessThanOrEqual,
        GreaterThanOrEqual,
        None,
    };

    ASTRelationalExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTRelationalExpression(ASTRelationalExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTRelationalExpression &operator=(ASTRelationalExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTEqualityExpression final : public ASTNode {
    enum class Operator {
        Equal,
        NotEqual,
        None,
    };

    ASTEqualityExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTEqualityExpression(ASTEqualityExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTEqualityExpression &operator=(ASTEqualityExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTAndExpression final : public ASTNode {
    enum class Operator {
        And,
        None,
    };

    ASTAndExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTAndExpression(ASTAndExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTAndExpression &operator=(ASTAndExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTExclusiveOrExpression final : public ASTNode {
    enum class Operator {
        ExclusiveOr,
        None,
    };

    ASTExclusiveOrExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTExclusiveOrExpression(ASTExclusiveOrExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTExclusiveOrExpression &operator=(ASTExclusiveOrExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTInclusiveOrExpression final : public ASTNode {
    enum class Operator {
        InclusiveOr,
        None,
    };

    ASTInclusiveOrExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTInclusiveOrExpression(ASTInclusiveOrExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTInclusiveOrExpression &operator=(ASTInclusiveOrExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTLogicalAndExpression final : public ASTNode {
    enum class Operator {
        LogicalAnd,
        None,
    };

    ASTLogicalAndExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTLogicalAndExpression(ASTLogicalAndExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTLogicalAndExpression &operator=(ASTLogicalAndExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTLogicalOrExpression final : public ASTNode {
    enum class Operator {
        LogicalOr,
        None,
    };

    ASTLogicalOrExpression(
            std::optional<Node> &&lhs,
            Operator operatorType,
            std::optional<Node> &&rhs
    )
        : lhs{std::move(lhs)}
        , operatorType{operatorType}
        , rhs{std::move(rhs)} {};

    ASTLogicalOrExpression(ASTLogicalOrExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);
    }

    ASTLogicalOrExpression &operator=(ASTLogicalOrExpression &&other) noexcept {
        this->lhs = std::move(other.lhs);
        this->operatorType = other.operatorType;
        this->rhs = std::move(other.rhs);

        return *this;
    }

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);

    static Operator OperatorFromTokenType(TokenType tokenType);

    std::optional<Node> lhs;
    Operator operatorType{};
    std::optional<Node> rhs;
};

struct ASTExpression final : public ASTNode {
    ASTExpression(Node &&first, std::vector<Node> &&other)
        : first{std::move(first)}
        , expressions{std::move(other)} {};

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &expr);

    Node first;
    std::vector<Node> expressions;
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
            Node &&list,
            Node &&identifier,
            Node &&expression
    )
        : identifier{std::move(identifier)}
        , specifier {std::move(list)}
        , expression{std::move(expression)} {};

    Node specifier;
    Node identifier;
    Node expression;

    [[nodiscard]]
    std::string ToString(int depth) const override;

    Node ToNode() override;

    static std::optional<Node> Match(Parser &parser);
};

#endif //JCC_ASTNODE_H
