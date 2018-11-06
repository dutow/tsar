
#include <catch.hpp>

#include <functional>
#include <iostream>

#include "tsar/standard_tuple.hpp"
#include "tsar/typewrap_literals.hpp"

using namespace tsar::literals;

static_assert(tsar::sorted_standard_tuple<int, bool, short>::offset<1>() == 6, "Offset usable and correct");
static_assert(alignof(tsar::sorted_standard_tuple<int, bool, short>) == 4, "Alignment is correct");

static_assert(tsar::standard_tuple<short, bool, bool, int, short, int>::offset<1>() == 2, "Offset usable and correct");
static_assert(tsar::standard_tuple<short, bool, bool, int, short, int>::offset<2>() == 3, "Offset usable and correct");
static_assert(tsar::standard_tuple<short, bool, bool, int, short, int>::offset<3>() == 4, "Offset usable and correct");
static_assert(tsar::standard_tuple<short, bool, bool, int, short, int>::offset<4>() == 8, "Offset usable and correct");
static_assert(tsar::standard_tuple<short, bool, bool, int, short, int>::offset<5>() == 12, "Offset usable and correct");
static_assert(alignof(tsar::standard_tuple<short, bool, bool, int, short, int>) == 4, "Alignment correct");
static_assert(alignof(tsar::standard_tuple<short, bool>) == 2, "Alignment correct");

TEST_CASE("Standard tuples can be constructed") {
  tsar::standard_tuple<int, bool, short> tup{42, false, 3};

  auto f0 = tup.get<0>();
  auto f1 = tup.get<1>();
  auto f2 = tup.get<2>();

  REQUIRE(f0 == 42);
  REQUIRE(f1 == false);
  REQUIRE(f2 == 3);
}

TEST_CASE("Standard tuples elements can be assigned") {
  tsar::standard_tuple<int, bool, short> tup{42, false, 3};

  auto& f0 = tup.get<0>();
  f0 += 5;

  REQUIRE(tup.get<0>() == 47);
  REQUIRE(tup.get(0_c) == 47);
}

TEST_CASE("Standard tuples have a size") {
  tsar::standard_tuple<int, bool, short> tup{42, false, 3};

  REQUIRE(decltype(tup)::size() == 3);
}

TEST_CASE("Standard tuple calls move when moved") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}
    test_type(test_type const& o) = delete;
    test_type(test_type&& o) noexcept : callback(std::move(o.callback)) { callback(); }

    test_type& operator=(test_type const& o) = delete;
    test_type& operator=(test_type&& o) = delete;
    ~test_type() = default;
  };

  test_type s{[&]() { call_count++; }};
  tsar::standard_tuple<test_type> tup{std::move(s)};

  REQUIRE(call_count == 1);
}

TEST_CASE("Standard tuple calls move when in place constructed") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}
    test_type(test_type const& o) = delete;
    test_type(test_type&& o) noexcept : callback(std::move(o.callback)) { callback(); }

    test_type& operator=(test_type const& o) = delete;
    test_type& operator=(test_type&& o) = delete;
    ~test_type() = default;
  };

  tsar::standard_tuple<test_type> tup{{[&]() { call_count++; }}};

  REQUIRE(call_count == 1);
}

TEST_CASE("Standard tuple calls copy when can't be moved") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}
    test_type(test_type const& o) noexcept : callback(o.callback) { callback(); }
    test_type(test_type&&) = delete;

    test_type& operator=(test_type const& o) = delete;
    test_type& operator=(test_type&& o) = delete;
    ~test_type() = default;
  };

  const test_type s{[&]() { call_count++; }};
  tsar::standard_tuple<test_type> tup{s};

  REQUIRE(call_count == 1);
}

TEST_CASE("Standard tuple destroys the objects") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}
    test_type(test_type const&) = default;
    test_type(test_type&&) = delete;

    test_type& operator=(test_type const& o) = delete;
    test_type& operator=(test_type&& o) = delete;
    ~test_type() { callback(); }
  };

  {
    const test_type s{[&]() { call_count++; }};
    tsar::standard_tuple<test_type> tup{s};
  }

  REQUIRE(call_count == 2);
}

TEST_CASE("Standard tuple can be copied") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}
    test_type(test_type const& o) noexcept : callback(o.callback) { callback(); }

    test_type(test_type&&) = delete;
    test_type& operator=(test_type const& o) = delete;
    test_type& operator=(test_type&& o) = delete;
    ~test_type() = default;
  };

  const test_type s{[&]() { call_count++; }};
  tsar::standard_tuple<test_type> tup{s};
  call_count = 0;
  auto other_tup = tup;  // NOLINT

  REQUIRE(call_count == 1);
}

TEST_CASE("Standard tuple can be moved") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}
    test_type(test_type&& o) noexcept : callback(std::move(o.callback)) { callback(); }

    test_type(test_type const&) = default;
    test_type& operator=(test_type const& o) = delete;
    test_type& operator=(test_type&& o) = delete;
    ~test_type() = default;
  };

  const test_type s{[&]() { call_count++; }};
  tsar::standard_tuple<test_type> tup{s};
  call_count = 0;
  auto other_tup = std::move(tup);  // NOLINT

  REQUIRE(call_count == 1);
}

TEST_CASE("Standard tuple can be copy assigned") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}
    test_type& operator=(test_type const& o) noexcept {
      callback = o.callback;
      callback();
      return *this;
    }

    test_type(test_type const&) = default;
    test_type(test_type&&) = delete;
    test_type& operator=(test_type&& o) = delete;
    ~test_type() = default;
  };

  const test_type s{[&]() { call_count++; }};
  tsar::standard_tuple<test_type> tup{s};
  auto other_tup = tup;
  call_count = 0;
  other_tup = tup;

  REQUIRE(call_count == 1);
}

TEST_CASE("Standard tuple can be move assigned") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}
    test_type& operator=(test_type&& o) noexcept {
      callback = std::move(o.callback);
      callback();
      return *this;
    }

    test_type(test_type const&) = default;
    test_type(test_type&&) = delete;
    test_type& operator=(test_type const& o) = delete;
    ~test_type() = default;
  };

  const test_type s{[&]() { call_count++; }};
  tsar::standard_tuple<test_type> tup{s};
  auto other_tup = tup;
  call_count = 0;
  other_tup = std::move(tup);

  REQUIRE(call_count == 1);
}

TEST_CASE("Standard tuple can be compared") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}

    bool operator==(test_type const& /* unused */) const {
      callback();
      return false;
    }
  };

  const test_type s{[&]() { call_count++; }};
  tsar::standard_tuple<test_type> tup{s};
  auto other_tup = tup;  // NOLINT

  REQUIRE(!(other_tup == tup));
  REQUIRE(call_count == 1);
}

TEST_CASE("Standard tuple can be non equal compared") {
  int call_count = 0;

  struct test_type {
    std::function<void()> callback;

    test_type(std::function<void()> callback) noexcept : callback(std::move(callback)) {}

    bool operator!=(test_type const& /* unused */) const {
      callback();
      return true;
    }
  };

  const test_type s{[&]() { call_count++; }};
  tsar::standard_tuple<test_type> tup{s};
  auto other_tup = tup;  // NOLINT

  REQUIRE(other_tup != tup);
  REQUIRE(call_count == 1);
}
