
#pragma once

#include <cstdlib>
#include <type_traits>

// Template metaprogramming "logarithmic search"
//
// Simpte utility for searching in long template series
// Benifits:
// * less template instantations than linear search
// * avoids the recursion limit in compilers
//
// It works in the entire size_t range by first jumping logarithmicly upwards
// (1,2,4,8,16...) until the condition is met, then performing a classic logarithmic
// search in the last range (if the lookup of N succeeded, but N*2 failed, in the
// N..N*2-1 range)

// Interface

namespace tsar {

// Returns the index of the first item where Lookup is false.
//
// The lookup assumes that the first N items are true, and that later
// items are all false. If this condition isn't met, the function will return
// incorrect results.
//
// Lookup<Idx, Id> should be a consteval lambda, returning a bool value,
// returning true if the condition is met for the index. The lookup sholudn't
// directly depend on Id, it is only used as a trick to allow changing return
// values.
//
// Id should be a different type every time this function is called to ensure
// repetitive lookups.
template <typename Lookup, typename Id>
consteval std::size_t log_search_v(Lookup const& l = Lookup{}, Id* i = nullptr);

// Simplified version which only requires a lambda
template <typename Lookup, auto Id = []() {}>
consteval std::size_t log_search(Lookup const& l = Lookup{});
}  // namespace tsar

// Implementation

namespace tsar::detail {

// Helper: Pow2 values in the entire size_t range
// ====================================
// The last value (pow2<64>) actually cheats, and holds 2**64-1, the largest value size_t can hold
//
// TODO: don't hardcode 64 as the size of size_t

constexpr const std::size_t size_t_bits = 64;

template <std::size_t I>
constexpr const std::size_t pow2 = pow2<I - 1> * 2;

template <>
inline constexpr std::size_t pow2<0> = 1;

template <>
inline constexpr std::size_t pow2<size_t_bits> = (pow2<size_t_bits - 1> - 1) * 2 + 1;

// Helper: does a lookup to enable Lookup based SFINAE
// ====================================
// (as direct use of Lookup at the below enable_if_t fails)
template <typename Lookup, std::size_t Idx, typename Id>
consteval bool do_lookup() {
  Lookup l{};
  return l.template operator()<Idx, Id>();
}

// Helper: jumps upwards until the condition is met
// ====================================
// Returns the last POW level (index for the pow2 template) which mets the condition

template <typename Lookup, std::size_t Curr_idx, std::size_t Pow_level, typename Id>
consteval std::size_t log_up_check(float) {
  return Pow_level;
}

template <typename Lookup, std::size_t Curr_idx, std::size_t Pow_level, typename Id,
          typename = std::enable_if_t<do_lookup<Lookup, pow2<Pow_level + 1>, Id>()>>
consteval std::size_t log_up_check(int) {
  return log_up_check<Lookup, pow2<Pow_level + 1>, Pow_level + 1, Id>(0);
}

// Helper: return the middle of a range
// ====================================

template <std::size_t Start, std::size_t End>
consteval std::size_t log_in_middle() {
  return Start + (End - Start)/2;
}

// Helper: logarithmic search within a range
// ====================================
// Returns the actual index of the last matching value

template <typename Lookup, std::size_t Start_idx, std::size_t End_idx, typename Id>
consteval std::size_t log_in_check(float);

template <typename Lookup, std::size_t Start_idx, std::size_t End_idx, typename Id,
          typename = std::enable_if_t<do_lookup<Lookup, log_in_middle<Start_idx, End_idx>(), Id>()>>
consteval std::size_t log_in_check(int) {
  if constexpr (Start_idx + 1 == End_idx) {
    return Start_idx;
  } else {
    // the middle exists -> it should be larger
    return log_in_check<Lookup, log_in_middle<Start_idx, End_idx>(), End_idx, Id>(0);
  }
}

template <typename Lookup, std::size_t Start_idx, std::size_t End_idx, typename Id>
consteval std::size_t log_in_check(float) {
  if constexpr (Start_idx + 1 == End_idx) {
    return Start_idx;
  } else {
    // the middle doesn't exists -> it should be smaller
    return log_in_check<Lookup, Start_idx, log_in_middle<Start_idx, End_idx>(), Id>(0);
  }
}

};  // namespace tsar::detail

namespace tsar {
template <typename Lookup, typename Id>
consteval std::size_t log_search_v(Lookup const& l, Id* i) {
  constexpr std::size_t pow_level_before = detail::log_up_check<Lookup, 0, 0, Id>(0);
  if constexpr (pow_level_before == 0) {
    if constexpr (detail::do_lookup<Lookup, 1, Id>()) {
      return 2;
    }
    if constexpr (detail::do_lookup<Lookup, 0, Id>()) {
      return 1;
    }
    return 0;
  } else {
    const std::size_t pow_level_after = pow_level_before + 1;
    return detail::log_in_check<Lookup, detail::pow2<pow_level_before>, detail::pow2<pow_level_after>, Id>(0) +
           1;
  }
}

// Only use this in non template ctx
template <typename Lookup, auto Id>
consteval std::size_t log_search(Lookup const& l) {
  return log_search_v(l, static_cast<decltype(Id)*>(nullptr));
}

}  // namespace tsar
