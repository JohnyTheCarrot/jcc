#ifndef PREPROCESSOR_TOKEN_H
#define PREPROCESSOR_TOKEN_H

#include "tokenizer/token.h"

namespace jcc::preprocessor {
    struct PreprocessorTokenBase {
        virtual ~PreprocessorTokenBase() = default;

        [[nodiscard]]
        virtual tokenizer::Token GetToken() const = 0;
    };

    class BasicPreprocessorToken final : public PreprocessorTokenBase {
        tokenizer::Token m_Token;

    public:
        explicit BasicPreprocessorToken(tokenizer::Token &&token);

        tokenizer::Token GetToken() const override;

        [[nodiscard]]
        bool operator==(BasicPreprocessorToken const &other) const noexcept;
    };

    class GluePreprocessorToken final : public PreprocessorTokenBase {
        using Lhs = std::variant<
                BasicPreprocessorToken, std::unique_ptr<GluePreprocessorToken>>;

        Lhs              m_Lhs;
        tokenizer::Token m_Rhs;

    public:
        GluePreprocessorToken(Lhs &&lhs, tokenizer::Token rhs);

        GluePreprocessorToken(GluePreprocessorToken const &);

        GluePreprocessorToken &operator=(GluePreprocessorToken const &);

        [[nodiscard]]
        GluePreprocessorToken Clone() const;

        tokenizer::Token GetToken() const override;

        [[nodiscard]]
        tokenizer::Token GetLhs() const;

        [[nodiscard]]
        tokenizer::Token GetRhs() const;

        [[nodiscard]]
        bool operator==(GluePreprocessorToken const &other) const noexcept;
    };

    using PreprocessorToken =
            std::variant<BasicPreprocessorToken, GluePreprocessorToken>;

    namespace pp_token {
        [[nodiscard]]
        tokenizer::Token GetToken(PreprocessorToken const &token);

        [[nodiscard]]
        tokenizer::Token GetBasicToken(PreprocessorToken const &token);
    }// namespace pp_token
}// namespace jcc::preprocessor

#endif//PREPROCESSOR_TOKEN_H
