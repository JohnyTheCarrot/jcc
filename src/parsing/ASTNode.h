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

struct ASTPostfixExpression : public ASTNode {
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

struct ASTStorageClassSpecifier : public ASTNode {
};

struct ASTDeclarationSpecifiers : public ASTNode {
};

struct ASTDeclaration : public ASTNode {

    explicit ASTDeclaration(const ASTTypeSpecifier &specifier, const ASTIdentifier &identifier, const ASTPrimaryExpression &constant) : identifier{identifier}, specifier{specifier},
                                                                                                                                        primaryExpression{constant } {};
    
    ASTTypeSpecifier specifier;
    ASTIdentifier identifier;
    ASTPrimaryExpression primaryExpression;

    [[nodiscard]]
    std::string ToString(int depth) const override {
        std::stringstream resultStream;

        resultStream <<         std::string(depth,     PRETTY_PRINT_CHAR)   << "ASTDeclaration {\n";
        resultStream <<         std::string(depth + 1, PRETTY_PRINT_CHAR)   << "specifier         = " << this->specifier.ToString(depth + 1) << ",\n";
        resultStream <<         std::string(depth + 1, PRETTY_PRINT_CHAR)   << "identifier        = " << this->identifier.ToString(depth + 1) << ",\n";
        resultStream <<         std::string(depth + 1, PRETTY_PRINT_CHAR)   << "primaryExpression = " << this->primaryExpression.ToString(depth + 1);
        resultStream << '\n' << std::string(depth,     PRETTY_PRINT_CHAR)   << '}';

        return resultStream.str();
    }
};

#endif //JCC_ASTNODE_H
