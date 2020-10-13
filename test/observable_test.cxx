
#include "catch.hpp"

#include <vector>

#include "tsar/cat.hpp"
#include "tsar/observable/observable.hpp"

using namespace tsar::observable;

struct marker_1 {};
struct marker_2 {};

template <typename T>
struct my_observer : public observer_t<T> {
  std::vector<T> events;
  void on_changed(T const& item) override { events.push_back(item); }
};

TEST_CASE("Observable primitive types fire on change") {
  auto o = tsar::cat{}
               .add<observer_registry>(observer_registry_marker{})
               .add<observable<int>::type>(marker_1{})
               .add<observable<int>::type>(marker_2{})
               .build();

  {
    auto observer1 = binding<my_observer<int>>(o.get(marker_1{}));
    auto observer2 = binding<my_observer<int>>(o.get(marker_1{}));
    auto observer3 = binding<my_observer<int>>(o.get(marker_2{}));

    o.get(marker_1{}) = 42;
    o.get(marker_1{}) = 42;
    o.get(marker_1{}) = 137;
    o.get(marker_1{}) = 42;

    REQUIRE(observer1.observer().events == std::vector<int>{42, 137, 42});
    REQUIRE(observer2.observer().events == std::vector<int>{42, 137, 42});
    REQUIRE(observer3.observer().events.empty());
  }

  o.get(marker_1{}) = 137;
}

struct marker_x {};
struct marker_y {};

template <typename CAT>
struct friendly_pos : public CAT {
  auto& x() { return CAT::get(marker_x{}); }
  auto& y() { return CAT::get(marker_y{}); }
};

TEST_CASE("Observable composite types fire on change") {
  using o_pos = decltype(tsar::cat{}.add<observable<int>::type>(marker_x{}).add<observable<int>::type>(marker_y{}));

  auto o_with_pos = tsar::cat{}
                        .add<observer_registry>(observer_registry_marker{})
                        .add<observable<o_pos>::type>(marker_1{})
                        .add<observable<o_pos, friendly_pos>::type>(marker_2{})
                        .build();

  {
    auto observer1 = binding<my_observer<int>>(o_with_pos.get(marker_1{}).get(marker_x{}));
    auto observer2 = binding<my_observer<int>>(o_with_pos.get(marker_1{}).get(marker_x{}));
    auto observer3 = binding<my_observer<int>>(o_with_pos.get(marker_1{}).get(marker_y{}));
    auto observer4 = binding<my_observer<int>>(o_with_pos.get(marker_2{}).get(marker_x{}));
    auto observer5 = binding<my_observer<int>>(o_with_pos.get(marker_2{}).get(marker_y{}));

    o_with_pos.get(marker_1{}).get(marker_x{}) = 1;
    o_with_pos.get(marker_1{}).get(marker_y{}) = 2;
    o_with_pos.get(marker_1{}).get(marker_x{}) = 3;
    o_with_pos.get(marker_2{}).x() = 4;
    o_with_pos.get(marker_2{}).y() = 5;
    o_with_pos.get(marker_2{}).x() = 6;
    o_with_pos.get(marker_2{}).y() = 7;
    o_with_pos.get(marker_1{}).get(marker_y{}) = 8;
    o_with_pos.get(marker_1{}).get(marker_x{}) = 9;

    REQUIRE(observer1.observer().events == std::vector<int>{1, 3, 9});
    REQUIRE(observer2.observer().events == std::vector<int>{1, 3, 9});
    REQUIRE(observer3.observer().events == std::vector<int>{2, 8});
    REQUIRE(observer4.observer().events == std::vector<int>{4, 6});
    REQUIRE(observer5.observer().events == std::vector<int>{5, 7});
  }
}

TEST_CASE("Strings can be observed") {
  auto o = tsar::cat{}
               .add<observer_registry>(observer_registry_marker{})
               .add<observable<std::string>::type>(marker_1{})
               .build();

  {
    auto observer1 = binding<my_observer<std::string>>(o.get(marker_1{}));

    o.get(marker_1{}) = "foo";
    o.get(marker_1{}) = "bar";
    o.get(marker_1{}) = "bar";
    o.get(marker_1{}) = "foo";

    REQUIRE(observer1.observer().events == std::vector<std::string>{"foo", "bar", "foo"});
  }
}
