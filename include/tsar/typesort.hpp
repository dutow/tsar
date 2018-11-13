
#pragma once

#include <array>
#include <tuple>

namespace tsar {

template <typename T, std::size_t order_t>
struct type_ordering {
  static const std::size_t order = order_t;
  using type = T;

  constexpr bool operator==(type_ordering) const { return true; }
};

template <typename... Tall>
constexpr std::size_t larger_or_earlier_types(std::size_t idx) {
  const std::size_t sizes[] = {alignof(Tall)...};
  const std::size_t size = sizes[idx];
  std::size_t match = 0;
  for (std::size_t i = 0; i < sizeof...(Tall); ++i) {
    if (sizes[i] > size || (sizes[i] == size && i < idx)) {
      match++;
    }
  }
  return match;
}

template <typename... Tall, std::size_t... Tidx>
constexpr auto descending_type_order_impl(std::index_sequence<Tidx...>) {
  return std::make_tuple(type_ordering<Tall, larger_or_earlier_types<Tall...>(Tidx)>{}...);
}

template <typename... Tall>
constexpr auto descending_type_order() {
  return descending_type_order_impl<Tall...>(std::make_index_sequence<sizeof...(Tall)>());
}

template <typename... Tall, std::size_t... Tidx>
constexpr auto original_type_order_impl(std::index_sequence<Tidx...>) {
  return std::make_tuple(type_ordering<Tall, Tidx>{}...);
}

template <typename... Tall>
constexpr auto original_type_order() {
  return original_type_order_impl<Tall...>(std::make_index_sequence<sizeof...(Tall)>());
}

// internal helper
template <typename T>
constexpr std::size_t idx_for_type_ordering() {
  return T::order;
}

template <typename... Tall>  // type_ordering<T>...
constexpr std::size_t idx_for_nth_type_in_order(std::size_t n, std::tuple<Tall...> = {}) {
  const std::size_t idxs[] = {idx_for_type_ordering<Tall>()...};
  for (std::size_t i = 0; i < sizeof...(Tall); ++i) {
    if (idxs[i] == n) {
      return i;
    }
  }
  return n;  // overindexing
}

template <std::size_t N, typename... Tall>  // type_ordering<T>...
constexpr auto type_ordering_for_nth_type_in_order(std::tuple<Tall...> = {}) {
  constexpr std::tuple<Tall...> tup;
  return std::get<idx_for_nth_type_in_order<Tall...>(N)>(tup);
}

template <std::size_t N, typename... Tall>  // type_ordering<T>...
constexpr std::size_t sizeof_for_nth_type_in_order() {
  return sizeof(typename decltype(type_ordering_for_nth_type_in_order<N, Tall...>())::type);
}

template <std::size_t N, typename... Tall>  // type_ordering<T>...
constexpr std::size_t alignof_for_nth_type_in_order() {
  return alignof(typename decltype(type_ordering_for_nth_type_in_order<N, Tall...>())::type);
}

struct order_info {
  std::size_t offset;
  std::size_t align;
  std::size_t size;
};

template <typename... Tall, std::size_t... Tidx>
constexpr std::array<order_info, sizeof...(Tall)> calculate_offsets_impl(std::index_sequence<Tidx...>) {
  using sarr_t = std::array<order_info, sizeof...(Tall)>;
  // non const operator = is non constexpr in std array
  using arr_t = order_info[sizeof...(Tall)];  // std::array<order_info, sizeof...(Tall)> ;

  arr_t ret = {
      order_info{0u, alignof_for_nth_type_in_order<Tidx, Tall...>(), sizeof_for_nth_type_in_order<Tidx, Tall...>()}...};
  for (std::size_t i = 1; i < sizeof...(Tall); ++i) {
    ret[i].offset = ret[i - 1].offset + ret[i - 1].size;
    if (ret[i].offset % ret[i].align != 0) {
      ret[i].offset += ret[i].align - (ret[i].offset % ret[i].align);
    }
  }
  return sarr_t{{{ret[Tidx]}...}};
}

template <typename... Tall>  // type_ordering<T>...
constexpr std::array<std::size_t, sizeof...(Tall)> calculate_indices(std::tuple<Tall...>) {
  return {{Tall::order...}};
}

template <typename... Tall>  // type_ordering<T>...
constexpr std::array<order_info, sizeof...(Tall)> calculate_offsets(std::tuple<Tall...>) {
  return calculate_offsets_impl<Tall...>(std::make_index_sequence<sizeof...(Tall)>());
}
}  // namespace tsar
