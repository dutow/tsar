
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

#include "tsar/cat.hpp"
#include "tsar/observable/observer_registry.hpp"

namespace tsar::observable {

template <typename CAT>
struct empty_wrap : public CAT {};

template <typename T, template <typename> typename UNUSED = empty_wrap>
class observable {
  static_assert("Type not supported by observable");
};

template <typename CTX, typename T>
class observable_immutable {
 public:
  observable_immutable() : data_() {}
  explicit observable_immutable(T const& data) : data_(data) {}

  template <size_t DUMMY>
  constexpr static size_t observable_increment() {
    return 1;
  }

  constexpr static size_t observable_offset() {
    return sum_counter<CTX::idx, typename CTX::tuple_t::std_tuple_t>::sum();
  }

  observable_immutable& operator=(T const& o) {
    if (data_ != o) {
      data_ = o;
      fire();
    }
    return *this;
  }

  template <typename CTX2>
  observable_immutable& operator=(observable_immutable<CTX2, T> const& o) {
    if (data_ != o.data_) {
      data_ = o.data_;
      fire();
    }
    return *this;
  }

  void observe(observer_t<T>& observer) { observer_registry().template observe<observable_offset()>(observer); }
  void unobserve(observer_t<T>& observer) { observer_registry().template unobserve<observable_offset()>(observer); }

  operator T() const { return data_; }

 private:
  T data_;

  auto& container() { return CTX::container(this); }
  auto& observer_registry() { return container().get(observer_registry_marker{}); }

  void fire() { observer_registry().template fire<observable_offset()>(*this); }
};

// A primitive-like (immutable) string implementation
// Not the most efficient, but it easily adapts strings:
// The only way to change them is to replace them completely
template <>
class observable<std::string> {
 public:
  template <typename CTX>
  class type : public observable_immutable<CTX, std::string> {
   public:
    using base_t = observable_immutable<CTX, std::string>;

    using base_t::base_t;
    using base_t::operator=;
  };
};

template <typename... T>
struct inside_march_t {};

template <template <typename> typename... ITEMS, typename... KEYS>
struct inside_march_t<detail::pair<KEYS, ITEMS>...> {
  inside_march_t(detail::kitten_march<detail::pair<KEYS, ITEMS>...>) {}

  template <typename CTX>
  static auto build() {
    return cat_recursion<CTX, detail::named_mapping<KEYS...>, ITEMS...>{};
  }
};

template <template <typename> typename... ITEMS, typename... KEYS>
auto inside_march(detail::kitten_march<detail::pair<KEYS, ITEMS>...> const& o) {
  return inside_march_t<detail::pair<KEYS, ITEMS>...>{o};
}

template <typename... T, template <typename> typename WRAP>
class observable<detail::kittens<T...>, WRAP> {
  using kittens_t = typename decltype(
      typename detail::kittens<T...>{}.template prepend<observer_forwarder>(observer_registry_marker{}))::march_t;
  using inside_t = decltype(inside_march(kittens_t{}));

 public:
  template <typename CTX>
  using type = WRAP<decltype(inside_t{kittens_t{}}.template build<CTX>())>;
};

template <typename CTX, typename T>
class observable_primitive : public observable_immutable<CTX, T> {
 public:
  static_assert(std::is_arithmetic_v<T>, "Only works with primitive arithmetic types");

  using base_t = observable_immutable<CTX, T>;

  using base_t::base_t;
  using base_t::operator=;
};

#define DECLARE_OBSERVABLE_PRIMITIVE(T)        \
  template <>                                  \
  class observable<T, empty_wrap> {            \
   public:                                     \
    template <typename CTX>                    \
    using type = observable_primitive<CTX, T>; \
  };

DECLARE_OBSERVABLE_PRIMITIVE(unsigned char);
DECLARE_OBSERVABLE_PRIMITIVE(unsigned short);
DECLARE_OBSERVABLE_PRIMITIVE(unsigned int);
#if MSVC
DECLARE_OBSERVABLE_PRIMITIVE(unsigned long);
#endif
DECLARE_OBSERVABLE_PRIMITIVE(std::uint64_t);
DECLARE_OBSERVABLE_PRIMITIVE(signed char);
DECLARE_OBSERVABLE_PRIMITIVE(signed short);
DECLARE_OBSERVABLE_PRIMITIVE(signed int);
#if MSVC
DECLARE_OBSERVABLE_PRIMITIVE(signed long);
#endif
DECLARE_OBSERVABLE_PRIMITIVE(std::int64_t);
DECLARE_OBSERVABLE_PRIMITIVE(bool);
DECLARE_OBSERVABLE_PRIMITIVE(float);
DECLARE_OBSERVABLE_PRIMITIVE(double);

#undef DECLARE_OBSERVABLE_PRIMITIVE

}  // namespace tsar::observable