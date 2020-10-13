
#include "tsar/field.hpp"

#include "tsar/assert.hpp"
#include "catch.hpp"
#include <iostream>

using namespace tsar;
using namespace tsar::assert;

TEST_CASE("Fields work, even with local types!") {
  TSAR_STRUCT(foo) {
    TSAR_FIELD(int, a);
    TSAR_FIELD(int, b) = 4;
    TSAR_FIELD(float, c);

    void a_custom_method() {}
  };

  foo f{};
  f.a = 10;

  REQUIRE(f.a == 10);
  REQUIRE(f.b == 4);

  f.c.enclosing().a = 22;

  REQUIRE(f.a == 22);
}

struct custom_type {
  int i;

  operator int() const {
    return i;
  }
};


TSAR_STRUCT(foo) {
  TSAR_FIELD(int, a);
  TSAR_FIELD(custom_type, b) {4};
  TSAR_FIELD(float, c);
};

static_assert(equals<foo::meta().size(), 3>());
static_assert(equals<foo::meta().name(), "foo"_s>());

TEST_CASE("Fields can be copied!") {
  foo f1{};
  foo f2{};
  f1.a = 10;

  f2 = f1;

  // shouldn't work
  //auto f = f1.a;

  REQUIRE(f2.a == 10);
}

TEST_CASE("Fields can be extracted as base types!") {
  foo f1{};
  f1.a = 10;

  // not int!
  long a = f1.a;

  REQUIRE(a == 10);
}

TEST_CASE("Fields can be extracted as convertible types!") {
  // Note: this doesn't work with final types
  foo f1{};

  REQUIRE(f1.b == 4);
  f1.b.i = 10;

  int a = f1.b;

  REQUIRE(a == 10);
}

TEST_CASE("Fields work with some pointer magic") {
  foo f1{};

  f1.b.i = 20;
  auto* ptr = &(f1.a.enclosing());
  ptr->b.i =  30;

  int a = f1.b.i;

  REQUIRE(a == 30);
}
