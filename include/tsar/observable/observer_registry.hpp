
#pragma once

#include "tsar/context_aware_tuple.hpp"
#include "tsar/detail/packed_ptr.hpp"

#include <type_traits>
#include <vector>

namespace tsar::observable {

struct observer_registry_marker {};

template <typename T>
class observer_t {
 public:
  virtual void on_changed(T const& item) = 0;
};

template <typename OBSERVER_T, typename OBSERVED_T>
class binding_t {
 public:
  binding_t(OBSERVED_T& observed) : observed_(observed) { observed_.observe(observer_); }
  ~binding_t() { observed_.unobserve(observer_); }

  OBSERVER_T& observer() { return observer_; }
  OBSERVER_T const& observer() const { return observer_; }

 private:
  OBSERVED_T& observed_;
  OBSERVER_T observer_;
};

template <typename OBSERVER_T, typename OBSERVED_T>
inline auto binding(OBSERVED_T& observed) {
  return binding_t<OBSERVER_T, OBSERVED_T>{observed};
}

template <size_t N, typename TT>
struct sum_counter {};

template <size_t N, typename... TT>
struct sum_counter<N, std::tuple<TT...>> {
  template <typename T>
  constexpr static size_t get_increment() {
    return T::template observable_increment<0>();
  }

  static const constexpr std::array<size_t, sizeof...(TT)> counts = {get_increment<TT>()...};

  constexpr static size_t sum() {
    size_t s = 0;
    for (int i = 0; i < N; ++i) {
      s += counts[i];
    }
    return s;
  }
};

template <typename CTX, typename MAPPING_T, template <typename> typename... T>
class cat_recursion : public context_aware_tuple_helper<MAPPING_T, cat_recursion<CTX, MAPPING_T, T...>,
                                                        std::make_index_sequence<sizeof...(T)>, T...>::type {
 public:
  using ctx_t = CTX;

  using base_t = typename context_aware_tuple_helper<MAPPING_T, cat_recursion<CTX, MAPPING_T, T...>,
                                                     std::make_index_sequence<sizeof...(T)>, T...>::type;

  using base_t::base_t;

  template <size_t DUMMY>
  constexpr static size_t observable_increment() {
    return base_t::size();
  }

  constexpr static size_t observable_offset() {
    return sum_counter<CTX::idx, typename CTX::tuple_t::std_tuple_t>::sum();
  }
};

// This is used in an inside CAT within another CAT
// forwards requests to the outer CAT
template <typename CTX>
class observer_forwarder {
 public:
  template <size_t IDX, typename T>
  void fire(T& item_ref) {
    outside_container().get(observer_registry_marker{}).template fire<IDX>(item_ref);
  }

  template <size_t DUMMY>
  constexpr static size_t observable_increment() {
    // since sum_counter is based on increment, this offsets everything based on the parent
    // Note: this is the function behind the "DUMMY".
    // Being a template delays instantation, as CTX::tuple_t is incomplete if this function body
    // is generated too soon
    return CTX::tuple_t::observable_offset();
  }

  constexpr static size_t observable_offset() { return decltype(CTX::container(nullptr))::observable_offset(); }

  template <size_t IDX, typename T>
  void observe(observer_t<T>& observer) {
    outside_container().get(observer_registry_marker{}).template observe<IDX>(observer);
  }

  template <size_t IDX, typename T>
  void unobserve(observer_t<T>& observer) {
    outside_container().get(observer_registry_marker{}).template unobserve<IDX>(observer);
  }

 private:
  auto& inside_container() { return CTX::container(this); }
  auto& outside_container() { return CTX::tuple_t::ctx_t::container(&inside_container()); }
};

template <typename CTX>
class observer_registry {
 public:
  ~observer_registry() {}

  template <size_t IDX, typename T>
  void fire(T& item_ref) {
    for (auto const& item : observers_) {
      if (item.data() == IDX) {
        observer_of_nth_type<IDX, T>(item.ptr())->on_changed(item_ref);
      }
    }
  }

  template <size_t DUMMY>
  constexpr static size_t observable_increment() {
    // TODO: 0?
    return 1;
  }

  constexpr static size_t observable_offset() {
    return sum_counter<CTX::idx, typename CTX::tuple_t::std_tuple_t>::sum();
  }

  template <size_t IDX, typename T>
  void observe(observer_t<T>& observer) {
    // TODO: static assert
    observers_.push_back(detail::packed_ptr{&observer, IDX});
  }

  template <size_t IDX, typename T>
  void unobserve(observer_t<T>& observer) {
    // TODO: static assert
    observers_.erase(std::remove(observers_.begin(), observers_.end(), detail::packed_ptr{&observer, IDX}),
                     observers_.end());
  }

 private:
  template <size_t IDX, typename T>
  static auto observer_of_nth_type(void* ptr) {
    using t = observer_t<T>;
    return static_cast<t*>(ptr);
  }

  std::vector<detail::packed_ptr> observers_;

  auto& container() { return CTX::container(this); }
};

}  // namespace tsar::observable