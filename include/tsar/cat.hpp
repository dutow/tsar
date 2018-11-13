
#pragma once

#include <cstddef>

#include "tsar/context_aware_tuple.hpp"
#include "tsar/detail/named_mapping.hpp"

namespace tsar {

namespace cat_detail {

template <typename KEY, template <typename> typename ITEM>
struct pair {
  using key_t = KEY;
  // using item_t = ITEM;
};

template <typename... T>
struct kitten_march {};

template <template <typename> typename... ITEMS, typename... KEYS>
struct kitten_march<pair<KEYS, ITEMS>...> {
  using tuple_t = generic_context_aware_tuple<detail::named_mapping<KEYS...>, ITEMS...>;

  static auto build() { return generic_context_aware_tuple<detail::named_mapping<KEYS...>, ITEMS...>{}; }
};

template <typename... ITEMS>
struct kittens {
  template <template <typename> typename ITEM, typename KEY>
  auto add(KEY k) {
    return kittens<ITEMS..., pair<KEY, ITEM>>{};
  }

  template <template <typename> typename ITEM, typename KEY>
  auto prepend(KEY k) {
    return kittens<pair<KEY, ITEM>, ITEMS...>{};
  }

  auto build() { return kitten_march<ITEMS...>::build(); }

  using march_t = kitten_march<ITEMS...>;
  using tuple_t = typename kitten_march<ITEMS...>::tuple_t;
};

}  // namespace cat_detail

struct cat {
  template <template <typename> typename ITEM, typename KEY>
  auto add(KEY k) {
    return cat_detail::kittens<cat_detail::pair<KEY, ITEM>>{};
  }
};

}  // namespace tsar