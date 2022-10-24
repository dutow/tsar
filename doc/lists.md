Lists
====

Tsar provides an easy to use type list, which is:

* bi directional
* expandable / stateful (functions always return results based on what's known at the line where the function is called)
* built upon standard C++20 features

Creating an empty list
---

```cpp
// Lists are created simply by defining a struct.
// Most list methods request this list id as a template parameter.
struct a_list{};
```

Adding items
---

```cpp
// List items require CRTP and inheritance, but the inheritance
// doesn't have to be public
class first_item: tsar::list::link<a_list, first_item> {};

// Elements are always added based on the list declaration,
// and not the latest item.
class second_item: tsar::list::link<a_list, second_item> {};

// List items can be anywhere: inside namespaces, inside functions,
// private inner classes, etc... they don't have to be publicly
// accessible everywhere
namespace foo {
  void a_function() {
    class third_item: tsar::list::link<a_list, second_item> {};
  }
}
```

Bi-directional access
---

```cpp
// Given an item in the list, we can query the item before or
// after it with next() and prev()
static_assert(std::is_same_v<
  decltype(tsar::list::next<a_list, first_item>()),
  // Function returning list types, or using list types as
  // parameters always use pointers
  second_item*
  >);

// Lists always have a head, an item before the first item
// Other than this, trying to get the item after the last or before
// the head results in a compilation error.
static_assert(std::is_same_v<
  decltype(tsar::list::prev<a_list, first_item>()),
  tsar::list::head<a_list>*
  >);

// We can also ask if there is an item before or after with
// has_prev / has_next. 
static_assert(tsar::list::has_next<a_list, first_item>);

// While we can ask for the head with prev, has_prev returns
// false here: head is not an item!
static_assert(!tsar::list::has_prev<a_list, first_item>);
```

Expansion / stateful metaprogramming support
---

Compared to most compile time libraries, Tsar lists employ a few tricks which forces the compiler to re-evaluate constexpr functions every time they are called.
This results in the following behavior:

```
// we do not have a 4th item
static_assert(!tsar::list::has_next<a_list, third_item>);

// let's add a forth item
class fourth_item: tsar::list::link<a_list, fourth_item> {};

// we DO have a 4th item now!
static_assert(tsar::list::has_next<a_list, third_item>);
```

This behavior is *true* for every list function.

Indexing
---

```cpp
// Lists are indexed, starting with 0
static_assert(tsar::list::index_of<a_list, first_item>() == 0);

// We can also request items based on an index
static_assert(std::is_same_v<
  decltype(tsar::list::at<a_list, 1>()),
  second_item*
  >);

// And the number of items can be queried with size
static_assert(tsar::list::size<a_list>() == 4);
```

For each
---

```cpp
// Lists also support a simple for each construct with a
// variadic template operator(), or simply a lambda
static_assert(4 = tsar::list::for_each<A_LIST>([](auto ...xs) {
  // All parameters are pointers!
  return sizeof(T)...; // number of items, alias size()
}));
```

