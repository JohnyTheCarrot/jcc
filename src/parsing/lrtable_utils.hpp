#ifndef LRTABLE_UTILS_H
#define LRTABLE_UTILS_H

#include <array>
#include <cstdint>
#include <optional>

namespace jcc::parsing {
    struct ActionRowElement final {
        enum class Action {
            Shift,
            Reduce,
            Accept,
            Error,
        };

        int    state;
        Action action;
    };

    consteval ActionRowElement Accept() {
        return {0, ActionRowElement::Action::Accept};
    }

    consteval ActionRowElement Error() {
        return {0, ActionRowElement::Action::Error};
    }

    consteval ActionRowElement Reduce(int state) {
        return {state, ActionRowElement::Action::Reduce};
    }

    consteval ActionRowElement Shift(int state) {
        return {state, ActionRowElement::Action::Shift};
    }

    using State = int;

    template<std::size_t nAction>
    using ActionRow = std::array<ActionRowElement, nAction>;

    template<std::size_t nGoto>
    using GotoRow = std::array<std::optional<State>, nGoto>;

    template<std::size_t nAction, std::size_t nGoto, std::size_t nStates>
    using LrOneTable =
            std::array<std::pair<ActionRow<nAction>, GotoRow<nGoto>>, nStates>;
}// namespace jcc::parsing

#endif//LRTABLE_UTILS_H
