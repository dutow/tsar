#include "tsar/log_search.hpp"

#include "tsar/assert.hpp"
using namespace tsar::assert;

// Tests about detail::pow2
// ========================================

static_assert(tsar::detail::pow2<0> == 1);
static_assert(tsar::detail::pow2<1> == 2);
static_assert(tsar::detail::pow2<2> == 4);
static_assert(tsar::detail::pow2<3> == 8);

// Tests about log_up_check
// ========================================

static const constexpr auto before_100 = []<std::size_t Idx, typename Id>() consteval { return Idx < 100; };

// Should be the same as the above lambda
struct before_100_s {
  template <std::size_t Idx, typename Id>
  consteval bool operator()() {
    return Idx < 100;
  }
};

struct Id1;  // Ids do not have to be defined, only declared
struct Id2;

// 2**6 == 128
static_assert(equals<tsar::detail::log_up_check<before_100_s, 0, 0, Id1>(0), 6>());
static_assert(equals<tsar::detail::log_up_check<decltype(before_100), 0, 0, Id1>(0), 6>());

#define UP_CHECK_TEST(true_until_before, expected_val)                                                    \
  static const constexpr auto before_##true_until_before = []<std::size_t Idx, typename Id>() consteval { \
    return Idx < true_until_before;                                                                       \
  };                                                                                                      \
  static_assert(equals<tsar::detail::log_up_check<decltype(before_##true_until_before), 0, 0, Id1>(0), expected_val>());

UP_CHECK_TEST(0, 0)  // Note that indexing should start at 1, this is the empty state
UP_CHECK_TEST(1, 0)  // This is also the empty state
UP_CHECK_TEST(2, 0)
UP_CHECK_TEST(3, 1)
UP_CHECK_TEST(4, 1)
UP_CHECK_TEST(5, 2)
UP_CHECK_TEST(16, 3)
UP_CHECK_TEST(17, 4)

// Tests about log_search itself
// ========================================

struct Id3;

static_assert(equals<tsar::log_search_v<before_100_s, Id3>(), 100>());
static_assert(equals<tsar::log_search_v<decltype(before_100), Id3>(), 100>());
static_assert(equals<tsar::log_search_v<decltype(before_0), Id3>(), 0>());
static_assert(equals<tsar::log_search_v<decltype(before_1), Id3>(), 1>());
static_assert(equals<tsar::log_search_v<decltype(before_2), Id3>(), 2>());
static_assert(equals<tsar::log_search_v<decltype(before_3), Id3>(), 3>());
static_assert(equals<tsar::log_search_v<decltype(before_4), Id3>(), 4>());
static_assert(equals<tsar::log_search_v<decltype(before_5), Id3>(), 5>());
static_assert(equals<tsar::log_search_v<decltype(before_16), Id3>(), 16>());
static_assert(equals<tsar::log_search_v<decltype(before_17), Id3>(), 17>());

static_assert(equals<tsar::log_search([]<std::size_t Idx, typename Id>() consteval { return Idx < 100; }), 100>());
static_assert(
    equals<tsar::log_search_v(
               []<std::size_t Idx, typename Id>() consteval { return Idx < 100; }, static_cast<Id1*>(nullptr)),
           100>());
