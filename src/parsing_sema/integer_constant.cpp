#include "integer_constant.h"

#include <magic_enum/magic_enum.hpp>

namespace jcc::parsing_sema {
    IntegerConstant::IntegerConstant(
            types::IntegerType type, std::int64_t value
    )
        : m_Type{type}
        , m_Value{value} {
    }

    void PrintTo(IntegerSuffix const &suffix, std::ostream *os) {
        *os << "IntegerSuffix{";
        if (suffix.m_Type.has_value()) {
            std::visit(
                    [&os](auto const &t) { PrintTo(t, os); },
                    suffix.m_Type.value()
            );
            *os << ", ";
        }

        *os << magic_enum::enum_name(suffix.m_Sign) << '}';
    }
}// namespace jcc::parsing_sema
