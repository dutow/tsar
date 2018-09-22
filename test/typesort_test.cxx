
#include "tsar/typesort.hpp"

static_assert(tsar::larger_or_earlier_types<int, int, double, int>(0) == 1);
static_assert(tsar::larger_or_earlier_types<int, int, double, int>(1) == 2);
static_assert(tsar::larger_or_earlier_types<int, int, double, int>(2) == 0);
static_assert(tsar::larger_or_earlier_types<int, int, double, int>(3) == 3);

struct ch3 {
  char c[3];
};

static const constexpr auto descending = tsar::descending_type_order<int, int, char, double, int, ch3>();
static_assert(std::get<0>(descending) == tsar::type_ordering<int, 1>{});
static_assert(std::get<1>(descending) == tsar::type_ordering<int, 2>{});
static_assert(std::get<2>(descending) == tsar::type_ordering<char, 4>{});
static_assert(std::get<3>(descending) == tsar::type_ordering<double, 0>{});
static_assert(std::get<4>(descending) == tsar::type_ordering<int, 3>{});
static_assert(std::get<5>(descending) == tsar::type_ordering<ch3, 5>{});

static_assert(tsar::idx_for_nth_type_in_order(0, descending) == 3);
static_assert(tsar::idx_for_nth_type_in_order(1, descending) == 0);
static_assert(tsar::idx_for_nth_type_in_order(2, descending) == 1);
static_assert(tsar::idx_for_nth_type_in_order(3, descending) == 4);
static_assert(tsar::idx_for_nth_type_in_order(4, descending) == 2);
static_assert(tsar::idx_for_nth_type_in_order(5, descending) == 5);

static_assert(tsar::type_ordering_for_nth_type_in_order<0>(descending) == tsar::type_ordering<double, 0>{});

static const constexpr auto indices = tsar::calculate_indices(descending);
static_assert(indices[0] == 1);
static_assert(indices[1] == 2);
static_assert(indices[2] == 4);
static_assert(indices[3] == 0);
static_assert(indices[4] == 3);
static_assert(indices[5] == 5);

static const constexpr auto ordered_offsets = tsar::calculate_offsets(descending);
static_assert(ordered_offsets.size() == 6);
static_assert(ordered_offsets[0].offset == 0);
static_assert(ordered_offsets[1].offset == 8);
static_assert(ordered_offsets[2].offset == 12);
static_assert(ordered_offsets[3].offset == 16);
static_assert(ordered_offsets[4].offset == 20);
static_assert(ordered_offsets[5].offset == 21);
static_assert(ordered_offsets[0].size == 8);
static_assert(ordered_offsets[1].size == 4);
static_assert(ordered_offsets[2].size == 4);
static_assert(ordered_offsets[3].size == 4);
static_assert(ordered_offsets[4].size == 1);
static_assert(ordered_offsets[5].size == 3);
static_assert(ordered_offsets[0].align == 8);
static_assert(ordered_offsets[1].align == 4);
static_assert(ordered_offsets[2].align == 4);
static_assert(ordered_offsets[3].align == 4);
static_assert(ordered_offsets[4].align == 1);
static_assert(ordered_offsets[5].align == 1);

int main() { return 0; }
