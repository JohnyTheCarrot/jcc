#ifndef CHAR_ITER_H
#define CHAR_ITER_H

#include <cstddef> // for size_t
#include <iosfwd>  // for istream, ostream
#include <iterator>// for input_iterator, input_iterator_tag
#include <memory>  // for shared_ptr
#include <variant> // for variant

#include "diagnostics/source.h"// for Source

namespace jcc::tokenizer {
    struct CharInfo final {
        std::size_t m_Pos{};
        char        m_Char{};
        bool        m_IsSentinel{false};

        [[nodiscard]]
        bool operator==(CharInfo const &other) const;

        friend void PrintTo(CharInfo const &charInfo, std::ostream *os);
    };

    class CharIter final {
        struct Sentinel final {
            std::size_t m_LastPos{};
        };

        using CurrentChar = std::variant<CharInfo, Sentinel>;

        std::istream                        *m_Input{};
        std::shared_ptr<diagnostics::Source> m_Source{};
        CurrentChar                          m_CurrentChar{Sentinel{}};
        std::size_t                          m_CurrentPos{};

        [[nodiscard]]
        bool Next();

        CharInfo Expect(char c);

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = CharInfo;
        using pointer           = value_type const *;
        using difference_type   = int;

        CharIter();

        CharIter(
                std::istream &input, std::shared_ptr<diagnostics::Source> source
        );

        explicit CharIter(char until);

        void SetInput(std::istream &input);

        [[nodiscard]]
        std::istream *GetInput() const;

        [[nodiscard]]
        std::shared_ptr<diagnostics::Source> GetSource() const;

        [[nodiscard]]
        Sentinel const &GetSentinel() const;

        [[nodiscard]]
        std::size_t GetCurrentPos() const;

        [[nodiscard]]
        value_type operator*() const;

        [[nodiscard]]
        pointer operator->() const;

        CharIter &operator++();

        CharIter operator++(int);

        [[nodiscard]]
        CharIter begin() const;

        static CharIter end();

        [[nodiscard]]
        bool operator==(CharIter const &other) const;

        [[nodiscard]]
        bool operator!=(CharIter const &other) const;

        static CharIter const c_UntilNewline;
    };

    static_assert(std::input_iterator<CharIter>);
}// namespace jcc::tokenizer

#endif//CHAR_ITER_H
