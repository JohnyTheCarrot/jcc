#ifndef JCC_CHARSTREAM_H
#define JCC_CHARSTREAM_H

#include <optional>
#include <string>

#include "Span.h"

namespace jcc {
    class InvalidBackslashException final : public std::exception {
    public:
        [[nodiscard]]
        char const *what() const noexcept override {
            return "Invalid backslash";
        }
    };

    class CharStream final {
        std::istream &m_IStream;
        char          m_Current{};
        bool          m_WasLastCharNewLine{false};

        void InternalNext();

        void NextLine() noexcept;

    public:
        using NextChar = std::optional<char>;

        static constexpr char END_OF_FILE{0xFF};
        SpanMarker            m_PreviousSpanMarker{};
        SpanMarker            m_CurrentSpanMarker{};

        explicit CharStream(std::istream &iStream)
            : m_IStream{iStream}
            , m_Current{'\0'} {
        }

        [[nodiscard]]
        bool Good() const noexcept;

        explicit operator bool() const noexcept;

        [[nodiscard]]
        bool
        operator!() const noexcept;

        [[nodiscard]]
        char
        operator*() const noexcept;

        [[nodiscard]]
        bool
        operator==(char c) const noexcept;

        [[nodiscard]]
        char Get() const noexcept;

        char Next();

        NextChar operator++();

        char operator++(int);
    };

    std::string &operator+=(std::string &str, std::optional<char> c);

    std::string &operator+=(std::string &str, CharStream const &charStream);
}// namespace jcc

#endif//JCC_CHARSTREAM_H
