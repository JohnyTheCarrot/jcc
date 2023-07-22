//
// Created by johny on 7/21/23.
//

#ifndef JCC_ASTNODE_H
#define JCC_ASTNODE_H


#include <variant>
#include <vector>
#include <memory>
#include <optional>

class Parser;

template<typename Child>
using ASTChild = std::unique_ptr<Child>;

template<typename Child>
using ASTOptionalChild = std::optional<ASTChild<Child>>;

template<typename Child>
using ASTChildren = std::vector<ASTChild<Child>>;

template<typename... Variants>
using ChildVariant = std::variant<ASTChild<Variants>...>;

struct ASTNode {
    virtual bool Parse(Parser &parser) = 0;
};

struct ASTFunctionSpecifier : public ASTNode {
    enum class Specifier {
        Inline,
        NoReturn,
    } specifier{};

    bool Parse(Parser &parser) override;
};

struct ASTTypeQualifier : public ASTNode {
    enum class TypeQualifier {
        Const,
        Restrict,
        Volatile,
        Atomic,
    } qualifier{};

    bool Parse(Parser &parser) override;
};

struct ASTTypeSpecifier : public ASTNode {
    bool Parse(Parser &parser) override;
};

struct ASTStorageClassSpecifier : public ASTNode {
    bool Parse(Parser &parser) override;
};

struct ASTDeclarationSpecifiers : public ASTNode {
    bool Parse(Parser &parser) override;
};

struct ASTDeclaration : public ASTNode {
    bool Parse(Parser &parser) override;
};


#endif //JCC_ASTNODE_H
