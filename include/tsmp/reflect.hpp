#pragma once
#include <cstdint>
#include <string_view>
#include <tuple>

namespace tsmp {

template <class T, class V>
struct field_description_t {
    using value_type = V;
    std::size_t id;
    std::string_view name;
    V T::* ptr;
};
template <class T, class V>
field_description_t(std::size_t, std::string_view,  V T::*) -> field_description_t<T, V>;

template <class T>
struct reflect {
    constexpr static bool reflectable = false;
    constexpr static auto fields() {
        return std::make_tuple();
    }
};

}

#ifndef INTROSPECT_PASS
#include "reflection.hpp"
#endif