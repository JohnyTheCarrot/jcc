#include "constant.h"

#include "misc/Diagnosis.h"
#include "numeric_constant.h"

namespace jcc::parsing_sema {
    // std::unique_ptr<AstPredefinedConstant>
    // ParsePredefinedConstant(tokenizer::Token const &token) noexcept {
    //     if (!token.Is<tokenizer::Keyword>())
    //         return nullptr;
    //
    //     auto const predefinedConstant{
    //             [&]() -> std::optional<PredefinedConstant> {
    //                 switch (std::get<tokenizer::Keyword>(token.m_Value)) {
    //                     case tokenizer::Keyword::True:
    //                         return PredefinedConstant::True;
    //                     case tokenizer::Keyword::False:
    //                         return PredefinedConstant::False;
    //                     case tokenizer::Keyword::Nullptr:
    //                         return PredefinedConstant::Nullptr;
    //                     default:
    //                         return std::nullopt;
    //                 }
    //             }()
    //     };
    //
    //     if (!predefinedConstant.has_value())
    //         return nullptr;
    //
    //     return std::make_unique<AstPredefinedConstant>(predefinedConstant.value(
    //     ));
    // }
    //
    // AstPredefinedConstant::AstPredefinedConstant(
    //         PredefinedConstant value
    // ) noexcept
    //     : m_Value{value} {
    // }

    // llvm::Value *AstPredefinedConstant::Codegen() {
    //     throw std::runtime_error{"Not implemented"};
    // }
    //
    // ValueCategory AstPredefinedConstant::GetValueCategory() const noexcept {
    //     return ValueCategory::RValue;
    // }

    AstExpressionPtr ParseConstant(tokenizer::Token &token) {
        if (token.Is<tokenizer::PpNumber>()) {
            return ParseNumericConstant(token);
        }

        if (token.Is<tokenizer::Keyword>()) {
            // May return nullptr, which is fine, it just means there's no syntactical match.
            // return ParsePredefinedConstant(token);
            return nullptr;
        }

        if (token.Is<tokenizer::CharacterConstant>()) {
            // TODO: Character constant
            // TODO: diagnosis
            throw FatalCompilerError{
                    // Diagnosis::Kind::TODO, std::move(token.m_Span)
            };
        }

        if (token.Is<tokenizer::Identifier>()) {
            // TODO: Enumeration constant
            return nullptr;// No TO DO exception to not conflict with variable resolution.
        }

        // Not a constant.
        return nullptr;
    }
}// namespace jcc::parsing_sema
