//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstFunctionSpecifier.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::optional<AstFunctionSpecifier> AstFunctionSpecifier::Parse(Parser &parser) {
        if (!parser)
            return std::nullopt;

        const Token &token{ parser.PeekNextToken() };

        FunctionSpecifierType functionSpecifierType;

        switch (token._type) {
            case TokenType::KeywordInline:
                functionSpecifierType = FunctionSpecifierType::Inline;
                break;
            case TokenType::KeywordNoreturn:
                functionSpecifierType = FunctionSpecifierType::Noreturn;
                break;
            default:
                return std::nullopt;
        }

        parser.AdvanceCursor();

        return AstFunctionSpecifier{ functionSpecifierType };
    }

    std::string AstFunctionSpecifier::ToString(size_t depth) const {
        TOSTRING_ONE_FIELD_ENUM(AstFunctionSpecifier, depth, _functionSpecifier)
    }
} // parsing