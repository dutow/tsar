
#include "catch.hpp"

#include "tsar/context_aware_tuple.hpp"
#include "tsar/typewrap_literals.hpp"

using namespace tsar::literals;

namespace {
template <typename CTX>
struct context_aware_a {
  int i = 42;

  context_aware_a() noexcept = default;
  context_aware_a(int i) noexcept : i(i) {}

  auto& container() { return CTX::container(this); }

  int first() { return container().template get<0>().i; }
  int second();
};

template <typename CTX>
struct context_aware_b : tsar::context_aware_helper<CTX> {
  int i;

  context_aware_b(int i) noexcept : i(i) {}

  auto& container() { return CTX::container(this); }

  int first() { return this->template get<0>().i; }
  int second() { return this->template get<1>().i; }
};
}  // namespace

TEST_CASE("Context aware tupes can be used with default constructors") {
  tsar::context_aware_tuple<context_aware_a, context_aware_a> tup;
  static_assert(sizeof(decltype(tup)) == 8);

  REQUIRE(tup.get<0>().first() == 42);
  REQUIRE(tup.get<0>().second() == 42);
  REQUIRE(tup.get<1>().first() == 42);
  REQUIRE(tup.get(1_c).second() == 42);
}

TEST_CASE("Context aware tupes can be used with non default constructors") {
  tsar::context_aware_tuple<context_aware_a, context_aware_a> tup{std::make_tuple(4), std::make_tuple(42)};

  REQUIRE(tup.get<0>().first() == 4);
  REQUIRE(tup.get<0>().second() == 42);
  REQUIRE(tup.get(1_c).first() == 4);
  REQUIRE(tup.get<1>().second() == 42);
}

TEST_CASE("Context aware tupes can be used with non default constructible types") {
  tsar::context_aware_tuple<context_aware_b, context_aware_b> tup{std::make_tuple(4), std::make_tuple(42)};

  REQUIRE(tup.get<0>().first() == 4);
  REQUIRE(tup.get<0>().second() == 42);
  REQUIRE(tup.get<1>().first() == 4);
  REQUIRE(tup.get<1>().second() == 42);
}

TEST_CASE("Context aware tupes can be moved") {
  tsar::context_aware_tuple<context_aware_b, context_aware_b> tup{std::make_tuple(4), std::make_tuple(42)};

  auto another_tup = std::move(tup);

  tup.get<0>().i = 55;  // NOLINT: bugprone-use-after-move

  REQUIRE(another_tup.get<0>().first() == 4);
  REQUIRE(another_tup.get(0_c).second() == 42);
  REQUIRE(another_tup.get<1>().first() == 4);
  REQUIRE(another_tup.get<1>().second() == 42);
}

TEST_CASE("Context aware tupes can be copied") {
  tsar::context_aware_tuple<context_aware_b, context_aware_b> tup{std::make_tuple(4), std::make_tuple(42)};

  auto another_tup = tup;

  tup.get<0>().i = 55;

  REQUIRE(another_tup.get<0>().first() == 4);
  REQUIRE(another_tup.get<0>().second() == 42);
  REQUIRE(another_tup.get<1>().first() == 4);
  REQUIRE(another_tup.get<1>().second() == 42);
}

template <typename CTX>
int context_aware_a<CTX>::second() {
  return container().template get<1>().i;
}
