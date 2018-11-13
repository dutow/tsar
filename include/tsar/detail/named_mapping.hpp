#pragma once

#include <array>
#include <cstddef>

#include "tsar/typewrap.hpp"

namespace tsar::detail {

template <typename T>
struct type_c {};

template <typename T1, typename T2>
inline constexpr bool operator==(type_c<T1> const& /* unused */, type_c<T2> const& /* unused */) {
  return std::is_same_v<T1, T2>;
}

template <typename T1, typename T2>
struct type_marker {
  static const constexpr bool it_is = type_c<T1>{} == type_c<T2>{};
};

template <typename NEEDLE, typename... HAYSTACK>
struct typelist_finder {
  static const constexpr std::array<bool, sizeof...(HAYSTACK)> markers = {type_marker<NEEDLE, HAYSTACK>::it_is...};

  static constexpr bool contains() {
    for (std::size_t i = 0; i < markers.size(); ++i) {
      if (markers[i]) {
        return true;
      }
    }
    return false;
  }

  static constexpr bool has_multiple() {
    std::size_t count = 0;
    for (std::size_t i = 0; i < markers.size(); ++i) {
      if (markers[i]) {
        count++;
      }
    }
    return count > 1;
  }

  static constexpr int index() {
    for (std::size_t i = 0; i < markers.size(); ++i) {
      if (markers[i]) {
        return i;
      }
    }
    return -1;
  }
};

template <typename... KEYS>
struct named_mapping {
  template <typename T>
  constexpr static auto index_for(T const& /* unused */) {
    using t = typelist_finder<T, KEYS...>;
    static_assert(t::contains(), "Type doesn't contain key");
    static_assert(!t::has_multiple(), "Duplicate key encountered");
    return index_for(int_t<t::index()>{});
  }

  template <int I>
  constexpr static auto index_for(int_t<I> const& /* unused */) {
    return I;
  }
};

}  // namespace tsar::detail