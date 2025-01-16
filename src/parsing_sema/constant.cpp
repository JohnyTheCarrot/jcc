#include "constant.h"

#include "misc/Diagnosis.h"

namespace jcc::parsing_sema {
    std::optional<PredefinedConstant>
    ParsePredefinedConstant(tokenizer::Token const &token) noexcept {
        if (!token.Is<tokenizer::Keyword>())
            return std::nullopt;

        switch (std::get<tokenizer::Keyword>(token.m_Value)) {
            case tokenizer::Keyword::True:
                return PredefinedConstant::True;
            case tokenizer::Keyword::False:
                return PredefinedConstant::False;
            case tokenizer::Keyword::Nullptr:
                return PredefinedConstant::Nullptr;
            default:
                return std::nullopt;
        }
    }

    AstNodePtr ParseConstant(tokenizer::Token &token) {
        if (token.Is<tokenizer::PpNumber>()) {
            return ParseNumericConstant(token);
        }

        if (auto const predefinedConstant{ParsePredefinedConstant(token)};
            predefinedConstant.has_value()) {
            throw FatalCompilerError{
                    Diagnosis::Kind::TODO, std::move(token.m_Span)
            };
        }

        return nullptr;
    }
}// namespace jcc::parsing_sema
