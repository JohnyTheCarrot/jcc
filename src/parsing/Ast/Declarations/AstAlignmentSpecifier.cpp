//
// Created by johny on 11/4/23.
//

#include "AstAlignmentSpecifier.h"
#include "../../Parser.h"
#include "AstTypeName.h"
#include "../Expressions/AstConstantExpression.h"

namespace parsing {
    std::optional<AstAlignmentSpecifier> AstAlignmentSpecifier::Parse(Parser &parser) {
        std::optional<Token> alignAsKw{ parser.ConsumeIfTokenIs(TokenType::KeywordAlignas) };

        if (!alignAsKw.has_value())
            return std::nullopt;

        parser.ExpectToken(TokenType::LeftParenthesis);
        std::unique_ptr<AstNode> inner{ nullptr };
        AlignmentSpecifierType specifierType;

        std::optional<AstTypeName> type{ AstTypeName::Parse(parser) };
        if (type) {
            specifierType = AlignmentSpecifierType::Type;
            inner = std::make_unique<AstTypeName>(*type);
            goto validResult;
        }

        inner = AstConstantExpression::Parse(parser);
        if (inner != nullptr) {
            specifierType = AlignmentSpecifierType::ConstantExpression;
            goto validResult;
        }

        parser.Error(alignAsKw->_span, "Expected type name or constant expression");
        // unreachable

    validResult:
        parser.ExpectToken(TokenType::RightParenthesis);

        return AstAlignmentSpecifier{ specifierType, std::move(inner) };
    }

    std::string AstAlignmentSpecifier::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstAlignmentSpecifier, depth, {
            TOSTRING_FIELD_ENUM("specifierType", _alignmentSpecifier)
            TOSTRING_FIELD_NODE("inner", *_alignmentSpecifierValue)
        })
    }
} // parsing