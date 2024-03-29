
#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

#include "tsar/compiler_support.hpp"

#include "tsar/cts.hpp"
#include "tsar/list.hpp"

namespace tsar {

#define TSAR_OFFSETOF offsetof

// NOTE: offset_of helper is complex because
// a. the member isn't even named in the source at this point
// b. we want this to work even as a local type within a function, where we can't define template functions
// To fix this:
// a. we have to return something template-ish, to delay instantiation until it's first used
// b. we have to use an auto lamdda, as that somehow isn't included in the "no templates" restriction
#define TSAR_FIELD_V(real_type, value_type, name, ...)                 \
  struct name##_offset_o {                                             \
    static TSAR_CONSTEVAL auto get() {                                      \
      return [](auto* T) {                                             \
        using TT = typename std::remove_reference<decltype(*T)>::type; \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Winvalid-offsetof\"") \
        return TSAR_OFFSETOF(TT, name);                                \
        _Pragma("GCC diagnostic pop") \
      };                                                               \
    }                                                                  \
  };                                                                   \
  using tsar_##name = real_type<struct_t, value_type, #name, name##_offset_o __VA_OPT__(, ) __VA_ARGS__>; \
  tsar_##name name

  // ^ the above using is only there to make error messages a bit more readable, maybe?

// TODO: make field_wrap customizable
#define TSAR_FIELD(type, name, ...) TSAR_FIELD_V(tsar::tsar_field_wrap, type, name, __VA_OPT__(, ) __VA_ARGS__)

// TODO: make field_wrap customizable!
#define TSAR_FIELD_T(type, name, ...) TSAR_FIELD_V(tsar::tsar_field_wrap_t, type, name, __VA_OPT__(, ) __VA_ARGS__)

#define TSAR_STRUCT(name)                           \
  struct name##_base;                               \
  using name = tsar::tsar_struct_wrap<name##_base>; \
  struct name##_base : public tsar::tsar_struct_base<name##_base, tsar::cts{#name}, tsar::tsar_struct_wrap>

#define TSAR_STRUCT2(name, wrap, ...)  \
  struct name##_base;             \
  using name = wrap<name##_base>; \
  struct name##_base : public tsar::tsar_struct_base<name##_base, tsar::cts{#name}, wrap> __VA_OPT__(,) __VA_ARGS__

#define TSAR_PROTECTED_COPY_AND_MOVE(T) \
 protected:                             \
  T(T const&) = default;                \
  T(T&&) = default;                     \
                                        \
  T& operator=(T const&) = default;     \
  T& operator=(T&&) = default;

#define TSAR_DELETE_COPY_AND_MOVE(T) \
 protected:                          \
  T(T const&) = delete;              \
  T(T&&) = delete;                   \
                                     \
  T& operator=(T const&) = delete;   \
  T& operator=(T&&) = delete;

template <typename ST_META_T, typename FIELD_WRAP_T, cts FIELD_NAME>
struct field_meta {
  TSAR_CONSTEVAL auto name() const { return FIELD_NAME; }
  TSAR_CONSTEVAL auto offset() const { return FIELD_WRAP_T::offset(); }
  TSAR_CONSTEVAL auto type() const { return static_cast<typename FIELD_WRAP_T::value_t*>(nullptr); }
};

template <typename T, cts NAME>
struct struct_meta {
  using list_t = typename T::tsar_struct_head;

  TSAR_CONSTEVAL auto name() const { return NAME; }

  TSAR_CONSTEVAL std::size_t size() const {
    return list::size(typename T::tsar_struct_head{}, []() {});
  }

  template <std::size_t IDX>
  TSAR_CONSTEVAL auto member_at() const {
    using item_t = std::remove_pointer_t<decltype(list::at<typename T::tsar_struct_head, IDX, []() {}>())>;
    return item_t::tsar_meta();
  }
};

template <typename T, tsar::cts NAME, template <typename> typename WRAP_T>
struct tsar_struct_base {
  using struct_t = T;
  using tsar_struct_t = tsar_struct_base;
  using tsar_struct_head = tsar::list::head<tsar_struct_t>;

  static const constexpr auto _name = NAME;

  auto wrapped_this() { return static_cast<WRAP_T<T>*>(this); }

  // + instance meta
};

template <typename T>
struct tsar_struct_wrap : public T {
  static TSAR_CONSTEVAL struct_meta<typename T::tsar_struct_t, cts<T::_name.size()>{T::_name}> meta() { return {}; }
};

template <typename STRUCT_T, typename TYPE_T, tsar::cts NAME, typename OFFSET>
struct tsar_field_wrap_composition {
  using value_t = TYPE_T;
  using enclosing_t = STRUCT_T;

  static TSAR_CONSTEVAL field_meta<STRUCT_T, tsar_field_wrap_composition, cts<NAME.size()>{NAME}> tsar_meta() { return {}; }

  static const TSAR_CONSTEVAL std::size_t offset() { return OFFSET::get()(static_cast<STRUCT_T*>(nullptr)); }

  TYPE_T t = 0;
  [[no_unique_address]] tsar::list::link<typename STRUCT_T::tsar_struct_head, tsar_field_wrap_composition> tsar_link;

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
  tsar_field_wrap_composition(tsar_field_wrap_composition&) = default;

  tsar_field_wrap_composition& operator=(tsar_field_wrap_composition const&) = default;
  tsar_field_wrap_composition& operator=(tsar_field_wrap_composition&) = default;

  friend STRUCT_T;
};

template <typename STRUCT_T, typename TYPE_T, tsar::cts NAME, typename OFFSET>
struct tsar_field_wrap_inheritance : public TYPE_T {
  using value_t = TYPE_T;
  using enclosing_t = STRUCT_T;

  [[no_unique_address]] tsar::list::link<typename STRUCT_T::tsar_struct_head, tsar_field_wrap_inheritance> tsar_link;

  using TYPE_T::TYPE_T;

  template <typename... Args>
  tsar_field_wrap_inheritance(Args&&... args) : TYPE_T(std::forward<Args>(args)...) {}

  template <typename... Args>
  tsar_field_wrap_inheritance& operator=(Args&&... args) {
    TYPE_T::operator=(std::forward<Args>(args)...);
    return *this;
  }


  static TSAR_CONSTEVAL field_meta<STRUCT_T, tsar_field_wrap_inheritance, cts<NAME.size()>{NAME}> tsar_meta() { return {}; }

  static const TSAR_CONSTEVAL std::size_t offset() { return OFFSET::get()(static_cast<STRUCT_T*>(nullptr)); }
  // static const TSAR_CONSTEVAL access(STRUCT_T const& t) { return t.CMPTR(); }
  // static TSAR_CONSTEVAL access(STRUCT_T& t) { return t.MPTR(); }

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


// TODO: make this customizable in a wrap configuration
// TODO: also make it a consteval function instead with if constexpr
template <typename STRUCT_T, typename TYPE_T, tsar::cts NAME, typename OFFSET>
struct tsar_field_wrap_helper {
  static TSAR_CONSTEVAL bool use_inheritance() { return std::is_class_v<TYPE_T> && !std::is_final_v<TYPE_T>; }

  using type = std::conditional_t<use_inheritance(),
                                  tsar_field_wrap_inheritance<STRUCT_T, TYPE_T, cts<NAME.size()>(NAME), OFFSET>,
                                  tsar_field_wrap_composition<STRUCT_T, TYPE_T, cts<NAME.size()>(NAME), OFFSET>>;
};

template <typename STRUCT_T, typename OFFSET>
struct wrap_magic {
  static const TSAR_CONSTEVAL std::size_t offset() { return OFFSET::get()(static_cast<STRUCT_T*>(nullptr)); }

  STRUCT_T& enclosing() { return *reinterpret_cast<STRUCT_T*>(reinterpret_cast<char*>(this) - offset()); }

  STRUCT_T const& enclosing() const {
    return *reinterpret_cast<const STRUCT_T*>(reinterpret_cast<const char*>(&this) - offset());
  }

  friend STRUCT_T;
};

template <typename T>
struct constructivity_check {
  static constexpr bool cc = std::is_copy_constructible_v<T>;
  static constexpr bool mc = std::is_move_constructible_v<T>;
  static constexpr bool ca = std::is_copy_assignable_v<T>;
  static constexpr bool ma = std::is_move_assignable_v<T>;
};

template <typename STRUCT_T, template <typename> typename TYPE_T, tsar::cts NAME, typename OFFSET>
struct tsar_field_wrap_t : public TYPE_T<wrap_magic<STRUCT_T, OFFSET>> {
  using value_t = TYPE_T<wrap_magic<STRUCT_T, OFFSET>>;
  using enclosing_t = STRUCT_T;

  static_assert(!constructivity_check<value_t>::cc, "Context aware types can't have public copy constructors");
  static_assert(!constructivity_check<value_t>::mc, "Context aware types can't have public move constructors");
  static_assert(!constructivity_check<value_t>::ca, "Context aware types can't have public copy assignment operators");
  static_assert(!constructivity_check<value_t>::ma, "Context aware types can't have public move assignment operators");

  [[no_unique_address]] tsar::list::link<typename STRUCT_T::tsar_struct_head, tsar_field_wrap_t> tsar_link;

  using value_t::value_t;

  static TSAR_CONSTEVAL field_meta<STRUCT_T, tsar_field_wrap_t, cts<NAME.size()>{NAME}> tsar_meta() { return {}; }

  static const TSAR_CONSTEVAL std::size_t offset() { return OFFSET::get()(static_cast<STRUCT_T*>(nullptr)); }

  enclosing_t& enclosing() { return *reinterpret_cast<enclosing_t*>(reinterpret_cast<char*>(this) - offset()); }

  enclosing_t const& enclosing() const {
    return *reinterpret_cast<const enclosing_t*>(reinterpret_cast<const char*>(&this) - offset());
  }

  template <typename... Args>
  tsar_field_wrap_t& operator=(Args&&... args) {
    value_t::operator=(std::forward<Args>(args)...);
    return *this;
  }

 private:
  // Try to keep these fields within the struct itself
  // The best way to do it is by disallowing construction from the outside

  tsar_field_wrap_t() = default;

  template <typename... Args>
  tsar_field_wrap_t(Args&&... args) : value_t(std::forward<Args>(args)...) {}

  tsar_field_wrap_t(tsar_field_wrap_t const& o) = default;
    
  tsar_field_wrap_t(tsar_field_wrap_t&&) = default;

  tsar_field_wrap_t& operator=(tsar_field_wrap_t const&) = default;
  tsar_field_wrap_t& operator=(tsar_field_wrap_t&&) = default;

  friend STRUCT_T;
};

template <typename STRUCT_T, typename TYPE_T, tsar::cts NAME, typename OFFSET>
using tsar_field_wrap = typename tsar_field_wrap_helper<STRUCT_T, TYPE_T, cts<NAME.size()>(NAME), OFFSET>::type;

}  // namespace tsar
