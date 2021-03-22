What's this?
===

`tsar` is a C++20 library providing easy to use compile time reflection with an easy DSL (no definition duplication required) and context (enclosing object) aware fields.

As it is required for it's implementation, it also happens to provide mutable compile time typelists with logarithmic search.

The original motivation to develop `tsar` was to create a pure C++ ORM, but it is also a generic library suited for anything requiring the above features, such as, for example:

* type safe GPU objects
* or easy integration of C++ objects to any scripting language
* UI frameworks, or anything requiring properties 

Compiler support
---

Currently `tsar` is only tested with GCC 11. Other compilers currently do not support required C++20 features.

A simple example
---

```cpp

// anywhere in the code, can even be local scope
TSAR_STRUCT(foo) {                // struct foo {
  TSAR_FIELD(int, a);             //   int a;
  TSAR_FIELD(custom_type, b) {4}; //   custom_type b {4};
  TSAR_FIELD(float, c);           //   float c;

  void a_custom_method() {}
};                                // };

// Reflection
static_assert(foo::meta().size() == 3);
static_assert(foo::meta().name() == "foo"_s);
static_assert(foo::meta().member_at<1>().name() == "b"_s);
static_assert(foo::meta().member_at<1>().offset() == 4);
static_assert(std::is_same_t<foo::meta().member_at<1>().type(), custom_type*>);

// Simple enclosing object access
foo f{};
f.c.enclosing().a = 22;
std::cout << f.a; // 22

```

Context aware fields
---

Tsar also allows users to write custom types that can access their enclosing objects:

```cpp
template<typename WRAP_T>
struct ctx_aware {
  int get_parent_a() {
    return static_cast<WRAP_T*>(this)->enclosing().a;
  }

  ctx_aware() = default;

  // Only the container type should be able to copy or move container aware types
  // otherwise there's a chance they could be moved outside accidentally.
  // If the type never has to be copied/moved, there's also
  // TSAR_DELETE_COPY_AND_MOVE(ctx_aware);
  TSAR_PROTECTED_COPY_AND_MOVE(ctx_aware);
};

TSAR_STRUCT(foo) {
  TSAR_FIELD(int, a);
  TSAR_FIELD_T(ctx_aware, b);
};

foo f{};
f.a = 42;

std::cout << f.b.get_parent_a(); // 42

auto bv = f.b; // compilation error
```
