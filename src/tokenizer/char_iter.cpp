#include "tokenizer/char_iter.h"

#include <format>
#include <utility>

#include "misc/Diagnosis.h"

namespace jcc::tokenizer {
    CharIter const CharIter::c_UntilNewline{'\n'};

    bool CharInfo::operator==(CharInfo const &other) const {
        return m_Char == other.m_Char && m_SpanMarker == other.m_SpanMarker;
    }

    bool CharIter::Next() {
        if (char next{}; m_Input->get(next).good()) {
            if (std::holds_alternative<Sentinel>(m_CurrentChar)) {
                m_CurrentChar = CharInfo{m_CurrentSpanMarker, next, false};
                return true;
            }

            m_CurrentSpanMarker.NextChar();

            auto &[spanMarker, character,
                   isSentinel]{std::get<value_type>(m_CurrentChar)};
            character  = next;
            spanMarker = m_CurrentSpanMarker;

            return true;
        }

        auto const lastSpanMarker{[this] {
            if (std::holds_alternative<CharInfo>(m_CurrentChar))
                return std::get<CharInfo>(m_CurrentChar).m_SpanMarker;

            return SpanMarker{};
        }()};

        m_CurrentChar = Sentinel{lastSpanMarker};
        return false;
    }

    CharInfo CharIter::Expect(char c) {
        if (!Next()) {
            auto const lastSpanMarker{
                    std::get<Sentinel>(m_CurrentChar).m_LastSpanMarker
            };
            Span span{m_FileName, lastSpanMarker, lastSpanMarker, m_Input};

            throw FatalCompilerError{
                    Diagnosis::Kind::UnexpectedEOF, std::move(span)
            };
        }

        if (auto const [spanMarker, character, isSentinel]{
                    std::get<value_type>(m_CurrentChar)
            };
            character != c) {
            Span span{m_FileName, spanMarker, spanMarker, m_Input};

            throw FatalCompilerError{
                    Diagnosis::Kind::TK_UnexpectedChar, std::move(span),
                    character
            };
        }

        return std::get<CharInfo>(m_CurrentChar);
    }

    CharIter::CharIter() = default;

    CharIter::CharIter(std::istream &input, std::string_view fileName)
        : m_Input{&input}
        , m_FileName{std::make_shared<std::string>(fileName)} {
        ++(*this);
    }

    CharIter::CharIter(char until)
        : m_CurrentChar{CharInfo{.m_Char = until, .m_IsSentinel = true}} {
    }

    void CharIter::SetInput(std::istream &input) {
        m_Input = &input;
    }

    std::istream *CharIter::GetInput() const {
        return m_Input;
    }

    std::shared_ptr<std::string> const &CharIter::GetFileName() const {
        return m_FileName;
    }

    CharIter::Sentinel const &CharIter::GetSentinel() const {
        return std::get<Sentinel>(m_CurrentChar);
    }

    SpanMarker const &CharIter::GetCurrentSpanMarker() const {
        if (std::holds_alternative<CharInfo>(m_CurrentChar)) {
            return std::get<CharInfo>(m_CurrentChar).m_SpanMarker;
        }

        return std::get<Sentinel>(m_CurrentChar).m_LastSpanMarker;
    }

    CharIter::value_type CharIter::operator*() const {
        return std::get<value_type>(m_CurrentChar);
    }

    CharIter::pointer CharIter::operator->() const {
        return &std::get<value_type>(m_CurrentChar);
    }

    CharIter &CharIter::operator++() {
        while (true) {
            if (!Next()) {
                return *this;
            }

            switch (auto nextChar{std::get<value_type>(m_CurrentChar)};
                    nextChar.m_Char) {
                case '\r':
                    Expect('\n');
                    m_CurrentSpanMarker.NextLine();
                    return *this;
                case '\n':
                    m_CurrentChar = nextChar;
                    m_CurrentSpanMarker.NextLine();
                    return *this;
                case '\t':
                    // See the following SO post for context. https://stackoverflow.com/a/13094734
                    // We're going up to 7 because the tab is already the first character
                    nextChar.m_SpanMarker.NextChar(false, 7);
                    return *this;
                case '\\':
                    switch (auto const next{m_Input->peek()}; next) {
                        case '\r':
                            m_Input->get();
                            Expect('\n');
                            m_CurrentSpanMarker.NextLine();
                            break;
                        case '\n':
                            m_Input->get();
                            m_CurrentSpanMarker.NextLine();
                            break;
                        default:
                            return *this;
                    }
                    continue;
                default:
                    return *this;
            }
        }
    }

    CharIter CharIter::operator++(int) {
        CharIter const copy{*this};
        ++(*this);
        return copy;
    }

    CharIter CharIter::begin() const {
        return CharIter{*m_Input, *m_FileName};
    }

    CharIter CharIter::end() {
        return CharIter{};
    }

    bool CharIter::operator==(CharIter const &other) const {
        if (std::holds_alternative<Sentinel>(m_CurrentChar) &&
            std::holds_alternative<Sentinel>(other.m_CurrentChar)) {
            return true;
        }

        if (std::holds_alternative<CharInfo>(m_CurrentChar) &&
            std::get<CharInfo>(m_CurrentChar).m_IsSentinel &&
            std::holds_alternative<Sentinel>(other.m_CurrentChar)) {
            return true;
        }

        if (std::holds_alternative<Sentinel>(m_CurrentChar) &&
            std::holds_alternative<CharInfo>(other.m_CurrentChar) &&
            std::get<CharInfo>(other.m_CurrentChar).m_IsSentinel) {
            return true;
        }

        if (!std::holds_alternative<CharInfo>(m_CurrentChar) ||
            !std::holds_alternative<CharInfo>(other.m_CurrentChar)) {
            return false;
        }

        auto const &currentChar{std::get<CharInfo>(m_CurrentChar)};
        auto const &otherChar{std::get<CharInfo>(other.m_CurrentChar)};

        if (currentChar.m_IsSentinel || otherChar.m_IsSentinel) {
            return currentChar.m_Char == otherChar.m_Char;
        }

        return m_Input == other.m_Input && currentChar == otherChar;
    }

    bool CharIter::operator!=(CharIter const &other) const {
        return !(*this == other);
    }

    void PrintTo(CharInfo const &charInfo, std::ostream *os) {
        *os << std::format(
                "CharInfo{{m_Char: {}, m_SpanMarker: {}}}",
                testing::PrintToString(charInfo.m_Char),
                testing::PrintToString(charInfo.m_SpanMarker)
        );
    }
}// namespace jcc::tokenizer
