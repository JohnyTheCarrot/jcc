#include "preprocessor_token.h"

#include <sstream>

#include "tokenizer/tokenizer.h"

namespace jcc::preprocessor {
    BasicPreprocessorToken::BasicPreprocessorToken(tokenizer::Token &&token)
        : m_Token{std::move(token)} {
    }

    tokenizer::Token BasicPreprocessorToken::GetToken() const {
        return m_Token;
    }

    bool BasicPreprocessorToken::operator==(BasicPreprocessorToken const &other
    ) const noexcept {
        return m_Token == other.m_Token;
    }

    GluePreprocessorToken::GluePreprocessorToken(
            Lhs &&lhs, tokenizer::Token rhs
    )
        : m_Lhs{std::move(lhs)}
        , m_Rhs{std::move(rhs)} {
    }

    GluePreprocessorToken::GluePreprocessorToken(
            GluePreprocessorToken const &other
    )
        : m_Lhs{[&other] -> Lhs {
            if (auto const lhs{std::get_if<BasicPreprocessorToken>(&other.m_Lhs)
                }) {
                return *lhs;
            }

            return std::make_unique<GluePreprocessorToken>(
                    *std::get<std::unique_ptr<GluePreprocessorToken>>(
                            other.m_Lhs
                    )
            );
        }()}
        , m_Rhs{other.m_Rhs} {
    }

    GluePreprocessorToken &
    GluePreprocessorToken::operator=(GluePreprocessorToken const &other) {
        if (this == &other)
            return *this;

        m_Lhs = [&other] -> Lhs {
            if (auto const lhs{std::get_if<BasicPreprocessorToken>(&other.m_Lhs)
                }) {
                return *lhs;
            }

            return std::make_unique<GluePreprocessorToken>(
                    *std::get<std::unique_ptr<GluePreprocessorToken>>(
                            other.m_Lhs
                    )
            );
        }();
        m_Rhs = other.m_Rhs;

        return *this;
    }

    GluePreprocessorToken GluePreprocessorToken::Clone() const {
        if (auto const lhs{std::get_if<BasicPreprocessorToken>(&m_Lhs)}) {
            return GluePreprocessorToken{*lhs, m_Rhs};
        }

        auto const &lhsUptr{
                std::get<std::unique_ptr<GluePreprocessorToken>>(m_Lhs)
        };

        auto clonedLhs{std::make_unique<GluePreprocessorToken>(lhsUptr->Clone())
        };
        return GluePreprocessorToken{std::move(clonedLhs), m_Rhs};
    }

    tokenizer::Token GluePreprocessorToken::GetToken() const {
        auto const lhs = [&] {
            if (auto const lhs{std::get_if<BasicPreprocessorToken>(&m_Lhs)}) {
                return lhs->GetToken();
            }

            return std::get<std::unique_ptr<GluePreprocessorToken>>(m_Lhs)
                    ->GetToken();
        }();

        std::string merged{std::format("{}{}", lhs.ToString(), m_Rhs.ToString())
        };
        std::istringstream   mergedStream{merged};
        tokenizer::Tokenizer mergerTokenizer{mergedStream};
        return mergerTokenizer.GetNextToken().value();
    }

    tokenizer::Token GluePreprocessorToken::GetLhs() const {
        if (auto const lhs{std::get_if<BasicPreprocessorToken>(&m_Lhs)}) {
            return lhs->GetToken();
        }

        return std::get<std::unique_ptr<GluePreprocessorToken>>(m_Lhs)->GetLhs(
        );
    }

    tokenizer::Token GluePreprocessorToken::GetRhs() const {
        return m_Rhs;
    }

    bool GluePreprocessorToken::operator==(GluePreprocessorToken const &other
    ) const noexcept {
        return m_Lhs == other.m_Lhs && m_Rhs == other.m_Rhs;
    }

    tokenizer::Token pp_token::GetToken(PreprocessorToken const &token) {
        return std::visit(
                [](auto const &ppToken) { return ppToken.GetToken(); }, token
        );
    }

    tokenizer::Token pp_token::GetBasicToken(PreprocessorToken const &token) {
        if (auto const basicToken{std::get_if<BasicPreprocessorToken>(&token)
            }) {
            return basicToken->GetToken();
        }

        throw std::runtime_error{"illegal glue"};
    }

}// namespace jcc::preprocessor
