
#pragma once

#include <cstddef>

#include "tsar/typewrap.hpp"

namespace tsar::literals {

namespace detail {

template <std::size_t N>
constexpr auto parse(const char (&digits)[N]) {
  int sum = 0;
  bool had_starter_digit = false;
  for (int i = 0; i < N; ++i) {
    switch (digits[i]) {
      case '0':
        if (had_starter_digit) {
          sum *= 10;
        }
        break;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (had_starter_digit) {
          sum *= 10;
        }
        sum += (1 + digits[i] - '1');
        had_starter_digit = true;
        break;
      default:
        break;
    }
  }

  return sum;
}
}  // namespace detail

template <char... c>
constexpr auto operator"" _c() {
  return int_t<detail::parse<sizeof...(c)>({c...})>{};
}

}  // namespace tsar::literals