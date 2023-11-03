//
// Created by johny on 10/19/23.
//

#include <sstream>
#include "AstConstant.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::optional<AstConstant> AstConstant::Parse(Parser &parser) {
        Token token{ parser.PeekNextToken() };

        switch(token._type) {
            case TokenType::IntegerLiteral:
                parser.AdvanceCursor();
                return AstConstant(std::get<IntegerLiteralTokenValue>(token._value));
            case TokenType::DoubleLiteral:
                assert(false && 'TODO');
            default:
                return std::nullopt;
        }
    }

    AstConstant::AstConstant(AstConstantValue constant)
        : AstNode(AstNodeType::Constant)
        , _value{constant} {

    }

    std::string AstConstant::ToString(size_t depth) const {
        std::stringstream ss;
        ss << "AstConstant{ ";

        if (std::holds_alternative<double>(_value))
            ss << "double: " << std::get<double>(_value);

        if (std::holds_alternative<IntegerLiteralTokenValue>(_value))
        {
            IntegerLiteralTokenValue value = std::get<IntegerLiteralTokenValue>(_value);

            ss << magic_enum::enum_name(value.type) << ": " << value.value;
        }

        ss << " }";



        return ss.str();
    }
} // parsing