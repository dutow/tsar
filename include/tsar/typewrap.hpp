
#pragma once

namespace tsar {

template <int I>
struct int_t {
  static const constexpr int value = I;

  static int constexpr val() { return I; }
};
}  // namespace tsar