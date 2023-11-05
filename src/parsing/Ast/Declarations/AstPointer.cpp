//
// Created by johny on 11/5/23.
//

#include <sstream>
#include "AstPointer.h"
#include "../../Parser.h"

namespace parsing {
    std::optional<AstPointer> AstPointer::Parse(Parser &parser) {
        AstPointer pointer{};

        std::optional<Token> asterisk{ parser.ConsumeIfTokenIs(TokenType::Asterisk) };

        if (!asterisk) {
            return std::nullopt;
        }

        Span span{ asterisk->_span };

        std::optional<AstTypeQualifierList> typeQualifierList{ AstTypeQualifierList::Parse(parser) };
        pointer._typeQualifierList = typeQualifierList;
        if (typeQualifierList) {
            span += typeQualifierList->_span;
        }

        std::optional<AstPointer> nextPointer{ AstPointer::Parse(parser) };
        if (!nextPointer) {
            pointer._nextPointer = nullptr;
            pointer._span = span;
            return pointer;
        }

        if (typeQualifierList) {
            span += nextPointer->_span;
        }

        pointer._span = span;
        pointer._nextPointer = std::make_unique<AstPointer>(std::move(*nextPointer));
        return pointer;
    }

    std::string AstPointer::ToString(size_t depth) const {
        std::stringstream ss;
        std::string tabs{ Indent(depth) };
        std::string tabsChildren{ Indent(depth + 1) };

        ss << "AstPointer {" << std::endl;

        if (_typeQualifierList) {
            ss << tabsChildren << "typeQualifierList: " << _typeQualifierList->ToString(depth + 1) << std::endl;
        }

        if (_nextPointer) {
            ss << tabsChildren << "nextPointer: " << _nextPointer->ToString(depth + 1) << std::endl;
        }

        ss << tabs << "}";

        return ss.str();
    }
} // parsing