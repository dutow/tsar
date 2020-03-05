
#pragma once

namespace tsar::assert {

template <auto... Args>
struct sum {
  consteval operator long long() const { return (... + Args); }
};

template <auto A, auto B>
struct equals {
  consteval operator bool() const {
    static_assert(A == B);
    return A == B;
  }
};

};  // namespace tsar::assert
