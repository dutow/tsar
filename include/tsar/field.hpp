
#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include "tsar/cts.hpp"
#include "tsar/list.hpp"

namespace tsar {

#define TSAR_OFFSETOF offsetof

// NOTE: offset_o helper os complex because
// a. the member isn't even named in the source at this point
// b. we want this to work even as a local type within a function, where we can't define template functions
// To fix this:
// a. we have to return something template-ish, to delay instantiation until it's first used
// b. we have to use an auto lamdda, as that somehow isn't included in the "no templates" restriction
#define TSAR_FIELD_V(real_type, value_type, name, ...)                 \
  struct name##_offset_o {                                             \
    static auto get() {                                                \
      return [](auto* T) {                                             \
        using TT = typename std::remove_reference<decltype(*T)>::type; \
        return TSAR_OFFSETOF(TT, name);                                \
      };                                                               \
    }                                                                  \
  };                                                                   \
  real_type<struct_t, value_type, #name, name##_offset_o __VA_OPT__(, ) __VA_ARGS__> name

#define TSAR_FIELD(type, name, ...) TSAR_FIELD_V(tsar::tsar_field_wrap, type, name, __VA_OPT__(, ) __VA_ARGS__)

#define TSAR_STRUCT(name)                     \
  struct name##_base;                         \
  using name = tsar::tsar_struct_wrap<name##_base>; \
  struct name##_base : public tsar::tsar_struct_base<name##_base, tsar::cts{#name}, tsar::tsar_struct_wrap>

#define TSAR_STRUCT2(name, wrap)  \
  struct name##_base;             \
  using name = wrap<name##_base>; \
  struct name##_base : public tsar::tsar_struct_base<name##_base, tsar::cts{#name}, wrap>

template <typename T, cts NAME>
struct struct_meta {
  constexpr auto name() const {
    return NAME;
  }


  constexpr std::size_t size() const {
    return list::size(typename T::tsar_struct_head{}, [](){});
  }

};

template <typename T, tsar::cts NAME, template<typename> typename WRAP_T>
struct tsar_struct_base {
  using struct_t = T;
  using tsar_struct_t = tsar_struct_base;
  using tsar_struct_head = tsar::list::head<tsar_struct_t>;

  static const constexpr auto _name = NAME;

  constexpr auto wrapped_this() {
    return static_cast<WRAP_T<T>*>(this);
  }

    // + instance meta
};

template <typename T>
struct tsar_struct_wrap : public T {
  static constexpr struct_meta<typename T::tsar_struct_t, cts<T::_name.size()>{T::_name}> meta() {
    return {};
  }
};

template <typename STRUCT_T, typename TYPE_T, tsar::cts NAME, typename OFFSET>
struct tsar_field_wrap_composition {  // TODO: only do this with primitives / final classes / unions
  using value_t = TYPE_T;
  using enclosing_t = STRUCT_T;
  static const constexpr std::size_t offset() { return OFFSET::get()(static_cast<STRUCT_T*>(nullptr)); }
  // static const constexpr access(STRUCT_T const& t) { return t.CMPTR(); }
  // static constexpr access(STRUCT_T& t) { return t.MPTR(); }
  //

  TYPE_T t = 0;
  [[no_unique_address]] tsar::list::link<typename STRUCT_T::tsar_struct_head, tsar_field_wrap_composition> l;

  tsar_field_wrap_composition() : t() {}

  tsar_field_wrap_composition(TYPE_T&& o) : t(o) {}

  template <typename... Args>
  tsar_field_wrap_composition(Args&&... args) : t(std::forward<Args>(args)...) {}

  TYPE_T& operator=(TYPE_T const& o) {
    t = o;
    return t;
  }

  operator TYPE_T&() { return t; }

  operator TYPE_T const &() const { return t; }

  enclosing_t& enclosing() { return *reinterpret_cast<enclosing_t*>(reinterpret_cast<char*>(this) - offset()); }

  enclosing_t const& enclosing() const {
    return *reinterpret_cast<const enclosing_t*>(reinterpret_cast<const char*>(&this) - offset());
  }

 private:
  // Try to keep these fields within the struct itself

  tsar_field_wrap_composition(tsar_field_wrap_composition const&) = default;
  tsar_field_wrap_composition(tsar_field_wrap_composition &) = default;

  tsar_field_wrap_composition& operator=(tsar_field_wrap_composition const&) = default;
  tsar_field_wrap_composition& operator=(tsar_field_wrap_composition &) = default;

  friend STRUCT_T;
};

template <typename STRUCT_T, typename TYPE_T, tsar::cts NAME, typename OFFSET>
struct tsar_field_wrap_inheritance : public TYPE_T {
  using value_t = TYPE_T;
  using enclosing_t = STRUCT_T;

  [[no_unique_address]] tsar::list::link<typename STRUCT_T::tsar_struct_head, tsar_field_wrap_inheritance> l;

  using TYPE_T::TYPE_T;

  template <typename... Args>
  tsar_field_wrap_inheritance(Args&&... args) : TYPE_T(std::forward<Args>(args)...) {}

  static const constexpr std::size_t offset() { return OFFSET::get()(static_cast<STRUCT_T*>(nullptr)); }
  // static const constexpr access(STRUCT_T const& t) { return t.CMPTR(); }
  // static constexpr access(STRUCT_T& t) { return t.MPTR(); }

  enclosing_t& enclosing() { return *reinterpret_cast<enclosing_t*>(reinterpret_cast<char*>(this) - offset()); }

  enclosing_t const& enclosing() const {
    return *reinterpret_cast<const enclosing_t*>(reinterpret_cast<const char*>(&this) - offset());
  }

 private:
  // Try to keep these fields within the struct itself

  tsar_field_wrap_inheritance(tsar_field_wrap_inheritance const&) = default;
  tsar_field_wrap_inheritance(tsar_field_wrap_inheritance&&) = default;

  tsar_field_wrap_inheritance& operator=(tsar_field_wrap_inheritance const&) = default;
  tsar_field_wrap_inheritance& operator=(tsar_field_wrap_inheritance&&) = default;

  friend STRUCT_T;
};

template <typename STRUCT_T, typename TYPE_T, tsar::cts NAME, typename OFFSET>
struct tsar_field_wrap_helper{
  static constexpr bool use_inheritance() {
    return std::is_class_v<TYPE_T> && !std::is_final_v<TYPE_T>;
  }

  using type = std::conditional_t<use_inheritance(), 
        tsar_field_wrap_inheritance<STRUCT_T, TYPE_T, cts<NAME.size()>(NAME), OFFSET>,
        tsar_field_wrap_composition<STRUCT_T, TYPE_T, cts<NAME.size()>(NAME), OFFSET>
          >;
};

template <typename STRUCT_T, typename TYPE_T, tsar::cts NAME, typename OFFSET>
using tsar_field_wrap = tsar_field_wrap_helper<STRUCT_T, TYPE_T, cts<NAME.size()>(NAME), OFFSET>::type;

}  // namespace tsar
