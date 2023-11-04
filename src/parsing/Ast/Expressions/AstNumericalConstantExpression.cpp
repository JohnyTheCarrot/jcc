//
// Created by johny on 10/19/23.
//

#include <sstream>
#include "AstNumericalConstantExpression.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::optional<AstNumericalConstantExpression> AstNumericalConstantExpression::Parse(Parser &parser) {
        Token token{ parser.PeekNextToken() };

        switch(token._type) {
            case TokenType::IntegerLiteral:
                parser.AdvanceCursor();
                return AstNumericalConstantExpression(std::get<IntegerLiteralTokenValue>(token._value));
            case TokenType::DoubleLiteral:
                assert(false && 'TODO');
            default:
                return std::nullopt;
        }
    }

    std::string AstNumericalConstantExpression::ToString(size_t depth) const {
        std::stringstream ss;
        ss << "AstNumericalConstantExpression{ ";

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