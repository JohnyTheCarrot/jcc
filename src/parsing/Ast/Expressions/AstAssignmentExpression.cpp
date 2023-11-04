//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstAssignmentExpression.h"
#include "AstConditionalExpression.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    std::unique_ptr<AstNode> AstAssignmentExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstConditionalExpression::Parse(parser) };

        if (left == nullptr)
            return nullptr;

        if (!parser)
            return left;

        const Token &token{ parser.PeekNextToken() };

        AstAssignmentOperator assignmentOperator;

        switch (token._type) {
            case TokenType::Assign:
                assignmentOperator = AstAssignmentOperator::Assign;
                break;
            case TokenType::MultiplyAssign:
                assignmentOperator = AstAssignmentOperator::MultiplyAssign;
                break;
            case TokenType::DivideAssign:
                assignmentOperator = AstAssignmentOperator::DivideAssign;
                break;
            case TokenType::ModuloAssign:
                assignmentOperator = AstAssignmentOperator::ModuloAssign;
                break;
            case TokenType::AddAssign:
                assignmentOperator = AstAssignmentOperator::PlusAssign;
                break;
            case TokenType::SubtractAssign:
                assignmentOperator = AstAssignmentOperator::MinusAssign;
                break;
            case TokenType::ShiftLeftAssign:
                assignmentOperator = AstAssignmentOperator::ShiftLeftAssign;
                break;
            case TokenType::ShiftRightAssign:
                assignmentOperator = AstAssignmentOperator::ShiftRightAssign;
                break;
            case TokenType::BitwiseAndAssign:
                assignmentOperator = AstAssignmentOperator::BitwiseAndAssign;
                break;
            case TokenType::ExclusiveOrAssign:
                assignmentOperator = AstAssignmentOperator::BitwiseXorAssign;
                break;
            case TokenType::BitwiseOrAssign:
                assignmentOperator = AstAssignmentOperator::BitwiseOrAssign;
                break;
            default:
                return left;
        }

        if (left->_hierarchyLevel >= Hierarchies::UnaryExpression) {
            parser.Error(token._span, "Invalid left hand side expression, expected lvalue.");
        }

        parser.AdvanceCursor();

        std::unique_ptr<AstNode> right{ AstAssignmentExpression::Parse(parser) };

        if (right == nullptr) {
            parser.Error(token._span, "Expected rhs expression");
        }

        return std::make_unique<AstAssignmentExpression>(std::move(left), assignmentOperator, std::move(right));
    }

    std::string AstAssignmentExpression::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstAssignmentExpression {" << std::endl;
        ss << tabsChildren << "left: " << _left->ToString(depth + 1) << std::endl;
        ss << tabsChildren << "assignmentOperator: " << magic_enum::enum_name(_assignmentOperator) << std::endl;
        ss << tabsChildren << "right: " << _right->ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing