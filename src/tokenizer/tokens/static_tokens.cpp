#include "static_tokens.h"

#include <cctype>  // for isspace
#include <optional>// for optional, operator==, nullopt
#include <utility> // for move

#include "misc/trie.h"          // for TrieNode
#include "tokenizer/char_iter.h"// for CharIter

namespace jcc::tokenizer::static_tokens {
    using TokenTrieValue =
            std::variant<Keyword, Directive, Punctuator, SpecialPurpose>;

    using KeywDirTokenTrie = TrieNode<'#', 'z', TokenTrieValue>;
    KeywDirTokenTrie const c_TokenTrie{
            {"auto", Keyword::Auto},
            {"break", Keyword::Break},
            {"case", Keyword::Case},
            {"char", Keyword::Char},
            {"const", Keyword::Const},
            {"constexpr", Keyword::Constexpr},
            {"continue", Keyword::Continue},
            {"default", Keyword::Default},
            {"do", Keyword::Do},
            {"double", Keyword::Double},
            {"else", Keyword::Else},
            {"enum", Keyword::Enum},
            {"extern", Keyword::Extern},
            {"float", Keyword::Float},
            {"for", Keyword::For},
            {"goto", Keyword::Goto},
            {"if", Keyword::If},
            {"inline", Keyword::Inline},
            {"int", Keyword::Int},
            {"long", Keyword::Long},
            {"register", Keyword::Register},
            {"restrict", Keyword::Restrict},
            {"return", Keyword::Return},
            {"short", Keyword::Short},
            {"signed", Keyword::Signed},
            {"sizeof", Keyword::Sizeof},
            {"static", Keyword::Static},
            {"struct", Keyword::Struct},
            {"switch", Keyword::Switch},
            {"typedef", Keyword::Typedef},
            {"union", Keyword::Union},
            {"unsigned", Keyword::Unsigned},
            {"void", Keyword::Void},
            {"volatile", Keyword::Volatile},
            {"while", Keyword::While},
            {"_Alignas", Keyword::Alignas},
            {"_Alignof", Keyword::Alignof},
            {"_Atomic", Keyword::Atomic},
            {"_Bool", Keyword::Bool},
            {"_Complex", Keyword::Complex},
            {"_Generic", Keyword::Generic},
            {"_Imaginary", Keyword::Imaginary},
            {"_Noreturn", Keyword::Noreturn},
            {"_Static_assert", Keyword::StaticAssert},
            {"_Thread_local", Keyword::ThreadLocal},
            {"false", Keyword::False},
            {"true", Keyword::True},
            {"nullptr", Keyword::Nullptr},
            // Directives
            {"#", Punctuator::Hash},
            {"##", Punctuator::HashHash},
            {"#include", Directive::Include},
            {"#define", Directive::Define},
            {"#undef", Directive::Undef},
            {"#line", Directive::Line},
            {"#error", Directive::Error},
            {"#warning", Directive::Warning},
            {"#pragma", Directive::Pragma},
            {"#if", Directive::If},
            {"#ifdef", Directive::Ifdef},
            {"#ifndef", Directive::Ifndef},
            {"#elif", Directive::Elif},
            {"#elifdef", Directive::Elifdef},
            {"#elifndef", Directive::Elifndef},
            {"#else", Directive::Else},
            {"#endif", Directive::Endif},
    };

    using PunctuatorTokenTrie = TrieNode<'!', '~', TokenTrieValue>;
    PunctuatorTokenTrie const c_PunctuatorTrie{
            {"[", Punctuator::LeftBracket},
            {"]", Punctuator::RightBracket},
            {"(", Punctuator::PpLeftParenthesis},
            {")", Punctuator::RightParenthesis},
            {"{", Punctuator::LeftBrace},
            {"}", Punctuator::RightBrace},
            {".", Punctuator::Dot},
            {"...", Punctuator::Ellipsis},
            {"-", Punctuator::Minus},
            {"--", Punctuator::MinusMinus},
            {"-=", Punctuator::MinusEqual},
            {"->", Punctuator::Arrow},
            {"+", Punctuator::Plus},
            {"++", Punctuator::PlusPlus},
            {"+=", Punctuator::PlusEqual},
            {"&", Punctuator::Ampersand},
            {"&&", Punctuator::AmpersandAmpersand},
            {"&=", Punctuator::AmpersandEqual},
            {"|", Punctuator::VerticalBar},
            {"||", Punctuator::VerticalBarVerticalBar},
            {"|=", Punctuator::VerticalBarEqual},
            {"^", Punctuator::Caret},
            {"^=", Punctuator::CaretEqual},
            {"<", Punctuator::LessThan},
            {"<<", Punctuator::LessThanLessThan},
            {"<=", Punctuator::LessThanEqual},
            {"<<=", Punctuator::LessThanLessThanEqual},
            {">", Punctuator::GreaterThan},
            {">>", Punctuator::GreaterThanGreaterThan},
            {">=", Punctuator::GreaterThanEqual},
            {">>=", Punctuator::GreaterThanGreaterThanEqual},
            {"=", Punctuator::Equal},
            {"==", Punctuator::EqualEqual},
            {",", Punctuator::Comma},
            {":", Punctuator::Colon},
            {";", Punctuator::Semicolon},
            {"?", Punctuator::QuestionMark},
            {"*", Punctuator::Asterisk},
            {"*=", Punctuator::AsteriskEqual},
            {"/", Punctuator::Slash},
            {"/=", Punctuator::SlashEqual},
            {"%", Punctuator::Percent},
            {"%=", Punctuator::PercentEqual},
            {"~", Punctuator::Tilde},
            {"<:", Punctuator::LeftBracket},
            {":>", Punctuator::RightBracket},
            {"<%", Punctuator::LeftBrace},
            {"%>", Punctuator::RightBrace},
            {"%:", Punctuator::Hash},
            {"%:%:", Punctuator::HashHash},
            {"!", Punctuator::ExclamationMark},
            {"!=", Punctuator::ExclamationMarkEqual},
            {"//", SpecialPurpose::LineComment},
            {"/*", SpecialPurpose::BlockComment},
            {"\\", SpecialPurpose::InvalidEscape},
    };

    [[nodiscard]]
    Token::Value TrieResultToTokenValue(TokenTrieValue value) {
        if (std::holds_alternative<Keyword>(value))
            return std::get<Keyword>(value);

        if (std::holds_alternative<Directive>(value))
            return std::get<Directive>(value);

        if (std::holds_alternative<SpecialPurpose>(value))
            return std::get<SpecialPurpose>(value);

        return std::get<Punctuator>(value);
    }

    StaticTokenTokenizationResult
    TokenizeKeywordsAndDirectives(CharIter &charIter) {
        Span span{
                charIter.GetSource(),
                {charIter.GetCurrentPos(), charIter.GetCurrentPos()}
        };
        std::string                   identifierBuf{};
        KeywDirTokenTrie const       *currentNode{&c_TokenTrie};
        std::optional<TokenTrieValue> trieResult{};

        while (charIter != CharIter::c_UntilNewline) {
            auto [spanMarker, character, isSentinel]{*charIter};

            if (character == '\'' || character == '"')
                break;

            auto [node, leaf]{currentNode->Find(character)};

            if (node != nullptr)
                identifierBuf.push_back(character);

            if (leaf != std::nullopt) {
                trieResult = leaf->m_Value;
            }

            span.m_Span.set_end(spanMarker);

            if (!std::isspace(character))
                currentNode = node;

            if (node == nullptr) {
                break;
            }

            ++charIter;
        }

        if (currentNode == nullptr ||
            !trieResult.has_value(
            ))// if we haven't reached the end of the trie or there's no trie result
            return {.valueOrString = std::move(identifierBuf),
                    .endPos        = span.m_Span.end()};

        return {.valueOrString = TrieResultToTokenValue(trieResult.value()),
                .endPos        = span.m_Span.end()};
    }

    StaticTokenTokenizationResult TokenizePunctuators(CharIter &charIter) {
        Span span{
                charIter.GetSource(),
                {charIter.GetCurrentPos(), charIter.GetCurrentPos()}
        };
        PunctuatorTokenTrie const    *currentNode{&c_PunctuatorTrie};
        std::optional<TokenTrieValue> trieResult{};

        while (charIter != CharIter::c_UntilNewline) {
            auto [spanMarker, character, isSentinel]{*charIter};

            auto [node, leaf]{currentNode->Find(character)};

            if (leaf != std::nullopt) {
                trieResult = leaf->m_Value;
            }

            span.m_Span.set_end(spanMarker);

            if (!std::isspace(character))
                currentNode = node;

            if (node == nullptr) {
                // if (charIter != CharIter::end())
                // 	++charIter;
                break;
            }

            ++charIter;
            span.m_Span.expand();
        }

        if (!trieResult.has_value())
            // TODO: diagnosis? do we need to diagnose this? does this even happen?
            throw diagnostics::FatalCompilerError{
                    // Diagnosis::Kind::TK_PartialTokenEncountered, std::move(span)
            };

        return {.valueOrString = TrieResultToTokenValue(trieResult.value()),
                .endPos        = span.m_Span.end()};
    }

    StaticTokenTokenizationResult Tokenize(CharIter &charIter) {
        if (auto [node, leaf]{c_PunctuatorTrie.Find(charIter->m_Char)};
            node == nullptr)
            return TokenizeKeywordsAndDirectives(charIter);

        return TokenizePunctuators(charIter);
    }
}// namespace jcc::tokenizer::static_tokens
