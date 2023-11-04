//
// Created by johny on 11/3/23.
//

#include <sstream>
#include "AstPostfixExpression.h"
#include "AstPrimaryExpression.h"
#include "../../Parser.h"
#include "AstExpression.h"
#include "../../../../libs/magic_enum/magic_enum.hpp"
#include "AstIdentifierExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstPostfixExpression::Parse(Parser &parser) {
        std::unique_ptr<AstNode> left{ AstPrimaryExpression::Parse(parser) };

        if (left == nullptr) {
            return nullptr;
        }

        while (true) {
            int parserCursor{ parser.GetCursor() };

            if (!parser)
                return left;

            const Token& nextToken{ parser.PeekNextToken() };

            switch (nextToken._type) {
                case TokenType::LeftBracket: {
                    parser.AdvanceCursor();

                    std::unique_ptr<AstNode> expression{ AstExpression::Parse(parser) };
                    std::optional<Token> rightBracket{ parser.ConsumeIfTokenIs(TokenType::RightBracket) };

                    if (expression == nullptr) {
                        parser.Error(nextToken._span, "Expected expression");
                    } else if (!rightBracket.has_value()) {
                        parser.Error(expression->_span, "Expected to be followed by ']'");
                    }

                    Span expressionSpan{ expression->_span };
                    left = std::make_unique<AstPostfixExpression>(std::move(left), PostfixExpressionType::ArraySubscript, std::move(expression));
                    left->_span = nextToken._span + expressionSpan + rightBracket->_span;
                    break;

                }
                case TokenType::Dot: {
                    parser.AdvanceCursor();

                    std::optional<AstIdentifierExpression> identifier{AstIdentifierExpression::Parse(parser) };

                    if (!identifier.has_value()) {
                        parser.Error(nextToken._span, "Expected identifier");
                    }

                    Span identifierSpan{ identifier->_span };
                    std::unique_ptr<AstNode> identifierNode{ std::make_unique<AstIdentifierExpression>(std::move(identifier.value())) };

                    left = std::make_unique<AstPostfixExpression>(std::move(left), PostfixExpressionType::MemberAccess, std::move(identifierNode));
                    left->_span = nextToken._span + identifierSpan;
                    break;
                }
                case TokenType::Arrow: {
                    parser.AdvanceCursor();

                    std::optional<AstIdentifierExpression> identifier{AstIdentifierExpression::Parse(parser) };

                    if (!identifier.has_value()) {
                        parser.Error(nextToken._span, "Expected identifier");
                    }

                    Span identifierSpan{ identifier->_span };
                    std::unique_ptr<AstNode> identifierNode{ std::make_unique<AstIdentifierExpression>(std::move(identifier.value())) };

                    left = std::make_unique<AstPostfixExpression>(std::move(left), PostfixExpressionType::PointerMemberAccess, std::move(identifierNode));
                    left->_span = nextToken._span + identifierSpan;
                    break;
                }
                case TokenType::Increment: {
                    parser.AdvanceCursor();

                    left = std::make_unique<AstPostfixExpression>(std::move(left), PostfixExpressionType::PostIncrement, nullptr);
                    left->_span = nextToken._span;
                    break;
                }
                case TokenType::Decrement: {
                    parser.AdvanceCursor();

                    left = std::make_unique<AstPostfixExpression>(std::move(left), PostfixExpressionType::PostDecrement, nullptr);
                    left->_span = nextToken._span;
                    break;
                }
                case TokenType::LeftParenthesis:
                    TODO()
                default:
                    parser.SetCursor(parserCursor);
                    return left;
            }
        }
    }

    std::string AstPostfixExpression::ToString(size_t depth) const {
        std::stringstream ss{};
        std::string tabs{ Indent(depth) };
        std::string childTabs{ Indent(depth + 1) };

        ss << "AstPostfixExpression {" << std::endl;
        ss << childTabs << "left: " << _left->ToString(depth + 1) << std::endl;
        ss << childTabs << "type: " << magic_enum::enum_name(_postfixExpressionType) << std::endl;
        if (_right != nullptr) {
            ss << childTabs << "right: " << _right->ToString(depth + 1) << std::endl;
        }
        ss << tabs << "}";

        return ss.str();
    }
} // parsing