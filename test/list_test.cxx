
#include "tsar/list.hpp"

#include "tsar/assert.hpp"

using namespace tsar::list;
using namespace tsar::assert;

struct A_LIST {};

using list_head = head<A_LIST>;

static_assert(equals<size(A_LIST{}, []() {}), 0>());
static_assert(!has_next<A_LIST, list_head>());
static_assert(!has_prev<A_LIST, list_head>());
// Normally index_of returns a std::size_t, but in this case it's signed. That shouldn't be an issue?
static_assert(equals<index_of<A_LIST, list_head>(), -1>());

struct S1 : public link<A_LIST, S1> {};
static_assert(std::is_same_v<decltype(next<A_LIST, list_head>()), S1*>);
static_assert(std::is_same_v<decltype(prev<A_LIST, S1>()), list_head*>);
static_assert(has_next<A_LIST, list_head>());
static_assert(!has_prev<A_LIST, list_head>());
static_assert(!has_next<A_LIST, S1>());
// While list_head::has_next returns true, we do not really want to travel to the head element frmo a real item
static_assert(!has_prev<A_LIST, S1>());
static_assert(equals<index_of<A_LIST, S1>(), 0>());

static_assert(equals<size(A_LIST{}, []() {}), 1>());

namespace foo {
void f_inner_test() {
  struct S1 : public link<A_LIST, S1> {};
  static_assert(has_prev<A_LIST, S1>());
  static_assert(equals<index_of<A_LIST, S1>(), 1>());
}
static_assert(equals<size(A_LIST{}, []() {}), 2>());
static_assert(has_next<A_LIST, S1>());

}  // namespace foo
