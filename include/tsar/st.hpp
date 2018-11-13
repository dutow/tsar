
#pragma once

#include <cstddef>

#include "tsar/detail/named_mapping.hpp"
#include "tsar/standard_tuple.hpp"

namespace tsar {

namespace st_detail {

template <typename KEY, typename ITEM>
struct pair {
  using key_t = KEY;
  // using item_t = ITEM;
};

template <ordering O, typename... T>
struct item_aggregator {};

template <ordering O, typename... ITEMS, typename... KEYS>
struct item_aggregator<O, pair<KEYS, ITEMS>...> {
  using tuple_t = generic_standard_tuple<O, detail::named_mapping<KEYS...>, standard_tuple_lifecycle_proxy, ITEMS...>;

  static auto build() {
    return generic_standard_tuple<O, detail::named_mapping<KEYS...>, standard_tuple_lifecycle_proxy, ITEMS...>{};
  }
};

template <ordering O, typename... ITEMS>
struct items {
  template <typename ITEM, typename KEY>
  auto add(KEY const& /* unused */) {
    return items<O, ITEMS..., pair<KEY, ITEM>>{};
  }

  auto build() { return item_aggregator<O, ITEMS...>::build(); }

  using tuple_t = typename item_aggregator<O, ITEMS...>::tuple_t;
};

}  // namespace st_detail

template <ordering O>
struct st {
  template <typename ITEM, typename KEY>
  auto add(KEY const& /* unused */) {
    return st_detail::items<O, st_detail::pair<KEY, ITEM>>{};
  }
};

}  // namespace tsar