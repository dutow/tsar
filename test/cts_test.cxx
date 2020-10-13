
#include "tsar/cts.hpp"

#include "catch.hpp"
#include <iostream>
#include <cstring>

using namespace tsar;

template<cts str>
struct S{
  constexpr auto get() const { return str; }
};

template<cts str>
// TODO: deduction bug?
// also, using str.str crases gcc
struct S2 : public S<cts<str.size()>{str}>{};

auto v = "asdf"_s;

//S2<"asdf2"_s> s2;


TEST_CASE("CTS contains the correct value") {
  constexpr S<"asdf"_s> s1;
  REQUIRE(strcmp("asdf", s1.get()) == 0);

  S<s1.get()> s2;

  REQUIRE(strcmp("asdf", s2.get()) == 0);
}

