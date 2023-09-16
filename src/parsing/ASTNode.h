//
// Created by johny on 7/21/23.
//

#ifndef JCC_ASTNODE_H
#define JCC_ASTNODE_H


#include <variant>
#include <vector>
#include <memory>
#include <optional>
#include "../Span.h"

class Parser;

template<typename Child>
using ASTChild = std::unique_ptr<Child>;

template<typename Child>
using ASTOptionalChild = std::optional<ASTChild<Child>>;

template<typename Child>
using ASTChildren = std::vector<ASTChild<Child>>;

template<typename... Variants>
using ChildVariant = std::variant<ASTChild<Variants>...>;

struct ParseResultError {
    Span span;
    std::string message;

    ParseResultError(const Span &span, std::string &&message) : span{span}, message{std::move(message)} {};
};

using ParseResult = std::optional<ParseResultError>;

struct ASTNode {
    virtual ParseResult Parse(Parser &parser) = 0;

    virtual ~ASTNode() = default;

    [[nodiscard]]
    std::unique_ptr<ASTNode> Clone() const;
};

struct ASTPrimaryExpression : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

struct ASTPostfixExpression : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

struct ASTExpression : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

struct ASTIdentifier : public ASTNode {
    std::string identifier;

    ParseResult Parse(Parser &parser) override;
};

struct ASTDeclarator : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

struct ASTInitDeclarator : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

struct ASTDeclaratorList : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

struct ASTFunctionSpecifier : public ASTNode {
    enum class Specifier {
        Inline,
        NoReturn,
    } specifier{};

    ParseResult Parse(Parser &parser) override;
};

struct ASTTypeQualifier : public ASTNode {
    enum class TypeQualifier {
        Const,
        Restrict,
        Volatile,
        Atomic,
    } qualifier{};

    ParseResult Parse(Parser &parser) override;
};

struct ASTTypeSpecifier : public ASTNode {
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

    ParseResult Parse(Parser &parser) override;
};

struct ASTStorageClassSpecifier : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

struct ASTDeclarationSpecifiers : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

struct ASTDeclaration : public ASTNode {
    ParseResult Parse(Parser &parser) override;
};

#endif //JCC_ASTNODE_H
