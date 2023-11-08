//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstInitializer.h"
#include "../../Parser.h"
#include "../Expressions/AstAssignmentExpression.h"

namespace parsing {
    std::unique_ptr<AstNode> AstInitializer::Parse(Parser &parser) {
        std::unique_ptr<AstNode> assignmentExpression{ AstAssignmentExpression::Parse(parser) };
        if (assignmentExpression)
            return assignmentExpression;

        std::optional<Token> lBrace{ parser.ConsumeIfTokenIs(TokenType::LeftBrace) };
        if (!lBrace)
            return nullptr;

        std::optional<AstInitializerList> initializerList{ AstInitializerList::Parse(parser) };
        if (!initializerList)
            parser.Error(lBrace->_span, "Expected to be followed by an initializer list");

        Span span{ lBrace->_span + initializerList->_span };
        Token comma;
        if (parser.AdvanceIfTokenIs(TokenType::Comma, comma))
            span += comma._span;

        parser.ExpectToken(TokenType::RightBrace);
        return std::make_unique<AstInitializer>(span, std::move(*initializerList));
    }

    std::string AstInitializer::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "Initializer {" << std::endl;
        ss << tabsChildren << _initializerList.ToString(depth + 1) << std::endl;
        ss << tabs << '}';

        return ss.str();
    }
} // parsing