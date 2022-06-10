
#include "tsar/field.hpp"

#include <iostream>

#include "catch.hpp"
#include "tsar/assert.hpp"

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

  // TODO: clang 14 fails to compile without a constructor
  // compiles fine if this constructor is here
  explicit custom_type(int i) : i(i) {}

  operator int() const { return i; }
};

TSAR_STRUCT(foo) {
  TSAR_FIELD(int, a);
  TSAR_FIELD(custom_type, b){4};
  TSAR_FIELD(float, c);
};

static_assert(equals<foo::meta().size(), 3>());
static_assert(equals<foo::meta().name(), "foo"_s>());
static_assert(equals<foo::meta().member_at<0>().name(), "a"_s>());
static_assert(equals<foo::meta().member_at<1>().name(), "b"_s>());
static_assert(equals<foo::meta().member_at<2>().name(), "c"_s>());
static_assert(equals<foo::meta().member_at<1>().offset(), 4>());
static_assert(equals<foo::meta().member_at<2>().offset(), 8>());
static_assert(std::is_same_v<decltype(foo::meta().member_at<1>().type()), custom_type*>);
static_assert(std::is_same_v<decltype(foo::meta().member_at<2>().type()), float*>);

TEST_CASE("Fields can be copied!") {
  foo f1{};
  foo f2{};
  f1.a = 10;

  f2 = f1;

  // shouldn't work
  // auto f = f1.a;

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
  ptr->b.i = 30;

  int a = f1.b.i;

  REQUIRE(a == 30);
}

template <typename WRAP_T>
struct ctx_aware : private WRAP_T {
  int get_parent_a() {  // not constexpr :(
    return this->enclosing().a;
  }

  using WRAP_T::WRAP_T;

  TSAR_PROTECTED_COPY_AND_MOVE(ctx_aware);

  // TSAR_PROTECTED_COPY_AND_MOVE(ctx_aware);
};

TEST_CASE("Containing type awareness") {
  TSAR_STRUCT(foo) {
    TSAR_FIELD(int, a);
    TSAR_FIELD_T(ctx_aware, b);
  };

  foo f{};
  f.a = 10;

  // compilation error!
  // ctx_aware t = f.b;

  REQUIRE(f.b.get_parent_a() == 10);
}

TEST_CASE("Containing type awareness and copies") {
  TSAR_STRUCT(foo) {
    TSAR_FIELD(int, a);
    TSAR_FIELD_T(ctx_aware, b);
  };

  static_assert(equals<foo::meta().member_at<1>().offset(), 4>());

  foo f{};
  f.a = 10;

  foo f2 = f;

  REQUIRE(f.b.get_parent_a() == 10);
}
