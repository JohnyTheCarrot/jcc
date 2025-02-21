#ifndef VISITOR_HPP
#define VISITOR_HPP

namespace jcc {
    template<typename... Ts>
    struct Visitor : Ts... {
        using Ts::operator()...;
    };
}// namespace jcc

#endif//VISITOR_HPP
