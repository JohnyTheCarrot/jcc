//
// Created by johny on 11/3/23.
//

#include <sstream>
#include "AstTypeQualifier.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::optional<AstTypeQualifier> AstTypeQualifier::Parse(Parser &parser) {
        if (!parser)
            return std::nullopt;

        const Token &token{ parser.PeekNextToken() };
        AstTypeQualifierType qualifierType;

        switch (token._type)
        {
            case TokenType::KeywordConst:
                qualifierType = AstTypeQualifierType::Const;
                break;
            case TokenType::KeywordRestrict:
                qualifierType = AstTypeQualifierType::Restrict;
                break;
            case TokenType::KeywordVolatile:
                qualifierType = AstTypeQualifierType::Volatile;
                break;
            case TokenType::KeywordAtomic:
                qualifierType = AstTypeQualifierType::Atomic;
                break;
            default:
                return std::nullopt;
        }

        parser.AdvanceCursor();

        return AstTypeQualifier{ token._span, qualifierType };
    }

    std::string AstTypeQualifier::ToString(size_t depth) const {
        std::stringstream ss;

        ss << "AstTypeQualifier(" << magic_enum::enum_name(_qualifierType) << ")";

        return ss.str();

    }
} // parsing