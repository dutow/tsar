
#include "catch.hpp"

#include "tsar/cat.hpp"

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

struct marker_1 {};
struct marker_2 {};

}  // namespace

TEST_CASE("Cats make nice tuples") {
  using t = decltype(tsar::cat{}.add<context_aware_a>(marker_1{}).add<context_aware_b>(marker_2{}));
  auto v = t::tuple_t{std::make_tuple(42), std::make_tuple(-42)};
  REQUIRE(v.get(marker_2{}).first() == 42);
  REQUIRE(v.size() == 2);
}

TEST_CASE("Cats make even nicer default constructible tuples") {
  using t = decltype(tsar::cat{}.add<context_aware_a>(marker_1{}).add<context_aware_a>(marker_2{}).build());
  auto v = t{};
  REQUIRE(v.get(marker_2{}).first() == 42);
  REQUIRE(v.size() == 2);
}