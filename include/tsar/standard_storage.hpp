
#pragma once

#include <tuple>
#include <type_traits>
#include "tsar/typesort.hpp"

namespace tsar {

enum class ordering { original, optimal };

template <ordering O, typename... T>
struct standard_storage_ordering;

template <typename... T>
struct standard_storage_ordering<ordering::optimal, T...> {
  static constexpr auto ordering = descending_type_order<T...>();
};

template <typename... T>
struct standard_storage_ordering<ordering::original, T...> {
  static constexpr auto ordering = original_type_order<T...>();
};

template <ordering O, typename... T>
class standard_storage : private standard_storage_ordering<O, T...> {
 private:
  static constexpr auto ordering = standard_storage_ordering<O, T...>::ordering;

 public:
  template <std::size_t IDX>
  using nth_type = typename std::remove_reference_t<decltype(std::get<IDX>(ordering))>::type;

  template <std::size_t IDX>
  auto& get() {
    return reinterpret_cast<nth_type<IDX>&>(data_[nth_offset(IDX)]);
  }

  template <std::size_t IDX>
  auto const& get() const {
    return reinterpret_cast<nth_type<IDX> const&>(data_[nth_offset(IDX)]);
  }

  template <std::size_t IDX>
  auto&& rvalue() {
    return reinterpret_cast<nth_type<IDX>&&>(data_[nth_offset(IDX)]);
  }

  template <std::size_t IDX>
  void* addr() {
    return static_cast<void*>(&data_[nth_offset(IDX)]);
  }

  constexpr static std::size_t size() { return sizeof...(T); }

  constexpr static std::size_t size_in_bytes() { return layout[size() - 1].offset + layout[size() - 1].size; }

  constexpr static std::size_t offset_for(std::size_t idx) {
    return offsetof(standard_storage, data_) + nth_offset(idx);
  }

 private:
  static constexpr auto indexing = calculate_indices(ordering);
  static constexpr auto layout = calculate_offsets(ordering);

  constexpr static std::size_t nth_offset(std::size_t idx) { return layout[indexing[idx]].offset; }

  static constexpr size_t maxalign() { return alignof(std::tuple<T...>); }

  alignas(maxalign()) char data_[size_in_bytes()];
};

}  // namespace tsar
