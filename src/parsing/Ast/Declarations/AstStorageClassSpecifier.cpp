//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstStorageClassSpecifier.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::optional<AstStorageClassSpecifier> AstStorageClassSpecifier::Parse(Parser &parser) {
        if (!parser)
            return std::nullopt;

        const Token &token{ parser.PeekNextToken() };

        StorageClassSpecifierType specifierType;

        switch (token._type) {
            case TokenType::KeywordTypedef:
                specifierType = StorageClassSpecifierType::Typedef;
                break;
            case TokenType::KeywordExtern:
                specifierType = StorageClassSpecifierType::Extern;
                break;
            case TokenType::KeywordStatic:
                specifierType = StorageClassSpecifierType::Static;
                break;
            case TokenType::KeywordThreadLocal:
                specifierType = StorageClassSpecifierType::ThreadLocal;
                break;
            case TokenType::KeywordAuto:
                specifierType = StorageClassSpecifierType::Auto;
                break;
            case TokenType::KeywordRegister:
                specifierType = StorageClassSpecifierType::Register;
                break;
            default:
                return std::nullopt;
        }

        parser.AdvanceCursor();

        return AstStorageClassSpecifier{ specifierType };
    }

    std::string AstStorageClassSpecifier::ToString(size_t depth) const {
        std::stringstream ss;

        ss << "AstStorageClassSpecifier(" << magic_enum::enum_name(_storageClassSpecifier) << ')';

        return ss.str();
    }
} // parsing