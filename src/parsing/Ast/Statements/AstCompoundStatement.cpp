//
// Created by johny on 11/8/23.
//

#include "AstCompoundStatement.h"
#include "../../Parser.h"

namespace parsing {
    std::optional<AstCompoundStatement> AstCompoundStatement::Parse(Parser &parser) {
        std::optional<Token> lBrace{ parser.ConsumeIfTokenIs(TokenType::LeftBrace) };
        if (!lBrace)
            return std::nullopt;

        std::optional<AstBlockItemList> blockItemList{ AstBlockItemList::Parse(parser) };
        const Token &rBrace{ parser.ExpectToken(TokenType::RightBrace) };

        Span span{lBrace->_span + rBrace._span};
        if (blockItemList)
            span += blockItemList->_span;

        return AstCompoundStatement(span, std::move(blockItemList));
    }

    std::string AstCompoundStatement::ToString(size_t depth) const {
        TOSTRING_FIELDS(AstCompoundStatement, depth, {
            if (_blockItemList)
                TOSTRING_FIELD_NODE("blockItemList", *_blockItemList)
        })
    }
} // parsing