
#pragma once

#include <cstdint>
#include <algorithm>

namespace tsar {

template<std::size_t N>
struct cts {
  char str[N] = {};

  constexpr cts()  {}
  constexpr cts(cts const&) = default;
  constexpr cts(const char (&s)[N])  {
    std::copy_n(s, N, str);
  }

  constexpr operator char const *() const { return str; }

  constexpr char const * c_str() const { return str; }

  constexpr std::size_t size() const { return N; }

  friend auto operator<=>(const cts&, const cts&) = default;
  friend bool operator==(const cts&, const cts&) = default;

};

template<cts c> constexpr auto operator ""_s() { return c; }

}  // namespace tsar
