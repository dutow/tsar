
#include "catch.hpp"

#include "tsar/detail/packed_ptr.hpp"

TEST_CASE("A packed ptr is able to store a value with save_data") {
  tsar::detail::packed_ptr pp;
  pp.save_data(42);
  REQUIRE(pp.data() == 42);
}