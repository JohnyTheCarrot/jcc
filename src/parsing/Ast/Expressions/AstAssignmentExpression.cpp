//
// Created by johny on 11/4/23.
//

#include <sstream>
#include "AstAssignmentExpression.h"
#include "AstConditionalExpression.h"
#include "../../Parser.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"

namespace parsing {
    AstNode::Ptr AstAssignmentExpression::Parse(Parser &parser) {
        AstNode::Ptr left{ AstConditionalExpression::Parse(parser) };

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

        AstNode::Ptr right{ AstAssignmentExpression::Parse(parser) };

        if (right == nullptr) {
            parser.Error(token._span, "Expected rhs expression");
        }

        return std::make_unique<AstAssignmentExpression>(std::move(left), assignmentOperator, std::move(right));
    }

    std::string AstAssignmentExpression::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstAssignmentExpression, depth, {
            TOSTRING_FIELD_NODE("left", *_left)
            TOSTRING_FIELD_ENUM("assignmentOperator", _assignmentOperator)
            TOSTRING_FIELD_NODE("right", *_right)
        })
    }
} // parsing