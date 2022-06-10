
#pragma once

#include <cstdlib>
#include <utility>

#include "tsar/log_search.hpp"
#include "tsar/compiler_support.hpp"


namespace tsar::list {

///////////////////////////////////////////

// Represents the head of a list.
// This struct is actually used by several internal functions to identify which list they have to work with
template <typename T>
struct head {
  static constexpr const long long int idx = -1;

  // Given an item (the first parameter) returns the next item
  template <typename Id>
  friend TSAR_CONSTEVAL auto next_link(head*, Id) noexcept;

  // Used for O(1) random access
  // Function is actually implemented when items are added
  template <long long Idx>
  struct index {
    template <typename Id>
    friend TSAR_CONSTEVAL auto get_nth(index<Idx>, Id) noexcept;
  };

  // All lists start with the head, the item before the first item
  template <typename Id>
  friend TSAR_CONSTEVAL auto get_nth(index<-1>, Id) noexcept {
    return static_cast<head<T>*>(nullptr);
  }
};

///////////////////////////////////////////

// True if the list already has N items
// Should be O(1) lookup

template <typename L, std::size_t N, typename Id,
          typename R = decltype(get_nth(typename head<L>::template index<N>{}, Id{}))>
TSAR_CONSTEVAL bool nth_exists(int) {
  return true;
};

template <typename L, std::size_t N, typename Id>
TSAR_CONSTEVAL bool nth_exists(float) {
  return false;
};

///////////////////////////////////////////

template <typename L, typename Id>
TSAR_CONSTEVAL std::size_t size(L, Id) {
  // TODO: why I have to specify the parameter here?
  auto l = []<std::size_t Idx, typename Id2>() { return nth_exists<L, Idx, Id2>(0); };
  return log_search_v<decltype(l), Id>();
}

///////////////////////////////////////////

// Links the next item

template <typename L, typename T>
struct link {
 public:
  static constexpr const std::size_t idx = size(L{}, []() {});

  template <typename Id>
  friend TSAR_CONSTEVAL auto get_nth(typename head<L>::template index<idx>, Id) noexcept {
    return static_cast<T*>(nullptr);
  }

  using prev_type = decltype(get_nth(typename head<L>::template index<static_cast<long long int>(idx) - 1>{}, []() {}));

  friend TSAR_CONSTEVAL prev_type prev_link(T*) noexcept { return static_cast<prev_type>(nullptr); }

  template <typename Id>
  friend TSAR_CONSTEVAL auto next_link(T*, Id) noexcept;

  template <typename Id>
  friend TSAR_CONSTEVAL auto next_link(prev_type, Id) noexcept {
    return static_cast<T*>(nullptr);
  }
};

// Public interface

// Return the element at a given index
// Compilation failure if there is no such element
// TODO: add a descriptive error about this
template <typename LL, long long Idx, auto Id = []() {}>
TSAR_CONSTEVAL auto at() {
  using idx_type = typename head<LL>::template index<Idx>;
  return get_nth(idx_type{}, Id);
}

// Return the next element
// Compilation failure if there is no such element
// TODO: add a descriptive error about this
template <typename LL, typename Curr, auto Id = []() {}>
TSAR_CONSTEVAL auto next() {
  return next_link(static_cast<Curr*>(nullptr), Id);
}

// Return the previous element
// Compilation failure if there is no such element
// TODO: add a descriptive error about this
template <typename LL, typename Curr>
TSAR_CONSTEVAL auto prev() {
  return prev_link(static_cast<Curr*>(nullptr));
}

// Return the index of an element
// Compilation failure if there is no such element
// TODO: add a descriptive error about this
template <typename LL, typename Curr, auto Id = []() {}>
TSAR_CONSTEVAL auto index_of() {
  return Curr::idx;
}

template <typename LL, typename Curr, auto Id = []() {}>
TSAR_CONSTEVAL bool has_prev() {
  const auto idx = Curr::idx;
  return idx > static_cast<decltype(idx)>(0);
}

template <typename LL, typename Curr, auto Id = []() {}>
TSAR_CONSTEVAL bool has_next() {
  // TODO: this could be optimized ...
  return static_cast<long long int>(index_of<LL, Curr, Id>()) < static_cast<long long int>(size(LL{}, Id)) - 1;
}

///////////////////////////////////////////
// These are not TSAR_CONSTEVAL:
// it's possible that F isn't TSAR_CONSTEVAL, and that's valid

template <typename L, typename F, auto Id, std::size_t... Is>
constexpr auto for_each_impl(F f, std::index_sequence<Is...>) {
  return f(at<L, Is, Id>()...);
}

// for_each<L, F, Id>
template <typename L, typename F, auto Id = [](){}, typename Indices = std::make_index_sequence<size(L{}, Id)>>
constexpr auto for_each(F f = {}) {
  return for_each_impl<L, F, Id>(f, Indices{});
}



}  // namespace tsar::list
