
#include "tsar/field.hpp"

using namespace tsar;

template <typename WRAP_T>
struct ctx_aware : private WRAP_T {
  int get_parent_a() {  // not constexpr :(
    return this->enclosing().a;
  }

  using WRAP_T::WRAP_T;

  TSAR_PROTECTED_COPY_AND_MOVE(ctx_aware);
};

  TSAR_STRUCT(foo) {
    TSAR_FIELD(int, a);
    TSAR_FIELD_T(ctx_aware, b);
  };

int main(int, char**) {
  // Should fail: context aware types shouldn't be constructible outside of the container type
  decltype(foo::b) extracted_field;
  return 0;
}
