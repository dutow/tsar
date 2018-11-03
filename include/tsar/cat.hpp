
#pragma once

#include <cstddef>

#include "tsar/context_aware_tuple.hpp"

namespace tsar {

namespace detail {

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

template <typename KEY, template <typename> typename ITEM>
struct pair {
  using key_t = KEY;
  // using item_t = ITEM;
};

template <typename... T>
struct kitten_march {};

template <template <typename> typename... ITEMS, typename... KEYS>
struct kitten_march<pair<KEYS, ITEMS>...> {
  using tuple_t = generic_context_aware_tuple<named_mapping<KEYS...>, ITEMS...>;

  static auto build() { return generic_context_aware_tuple<named_mapping<KEYS...>, ITEMS...>{}; }
};

template <typename... ITEMS>
struct kittens {
  template <template <typename> typename ITEM, typename KEY>
  auto add(KEY k) {
    return kittens<ITEMS..., pair<KEY, ITEM>>{};
  }

  auto build() { return kitten_march<ITEMS...>::build(); }

  using tuple_t = typename kitten_march<ITEMS...>::tuple_t;
};

}  // namespace detail

struct cat {
  template <template <typename> typename ITEM, typename KEY>
  auto add(KEY k) {
    return detail::kittens<detail::pair<KEY, ITEM>>{};
  }
};

}  // namespace tsar