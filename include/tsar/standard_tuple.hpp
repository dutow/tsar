#include "tsar/standard_storage.hpp"

namespace tsar {

template <typename T>
struct standard_tuple_lifecycle_proxy {
  static int construct(void* addr) {
    new (addr) T();
    return 0;
  }

  template <typename... TA>
  static int construct(void* addr, TA&&... args) {
    new (addr) T(std::forward<TA>(args)...);
    return 0;
  }

  template <typename... TA>
  static int construct(void* addr, TA const&... args) {
    new (addr) T(args...);
    return 0;
  }

  static int assign(T& to, T&& from) {
    to = std::forward<T>(from);
    return 0;
  }

  static int assign(T& to, T const& from) {
    to = from;
    return 0;
  }

  static int destruct(T* object) {
    object->~T();
    return 0;
  }
};

template <template <typename> typename LIFECYCLE_T, typename I, typename... T>
class standard_tuple_impl;

template <template <typename> typename LIFECYCLE_T, size_t... Is, typename... T>
class standard_tuple_impl<LIFECYCLE_T, std::index_sequence<Is...>, T...> {
 private:
  using storage_t = standard_storage<T...>;
  storage_t data_;

  template <typename... Ts>
  void swallow(Ts&&...) {}

  template <size_t IDX>
  void* addr() {
    static_assert(IDX < size(), "Overindexing a standard tuple");
    return reinterpret_cast<uint8_t*>(this) + offset<IDX>();
  }

  template <size_t IDX>
  auto&& move_out() {
    static_assert(IDX < size(), "Overindexing a standard tuple");
    return data_.template rvalue<IDX>();
  }

 public:
  standard_tuple_impl() noexcept { swallow(LIFECYCLE_T<T>::construct(addr<Is>())...); }

  template <typename... TT>
  standard_tuple_impl(TT&&... args) noexcept {
    swallow(LIFECYCLE_T<T>::construct(addr<Is>(), std::forward<TT>(args))...);
  }

  template <typename... TT>
  standard_tuple_impl(TT const&... args) noexcept {
    swallow(LIFECYCLE_T<T>::construct(addr<Is>(), args)...);
  }

  standard_tuple_impl(T&&... args) noexcept {
    swallow(LIFECYCLE_T<T>::construct(addr<Is>(), std::forward<T>(args))...);
  }

  standard_tuple_impl(T const&... args) noexcept { swallow(LIFECYCLE_T<T>::construct(addr<Is>(), args)...); }

  standard_tuple_impl(standard_tuple_impl const& o) noexcept {
    swallow(LIFECYCLE_T<T>::construct(addr<Is>(), o.get<Is>())...);
  }

  standard_tuple_impl(standard_tuple_impl&& o) noexcept {
    swallow(LIFECYCLE_T<T>::construct(addr<Is>(), o.move_out<Is>())...);
  }

  ~standard_tuple_impl() noexcept { swallow(LIFECYCLE_T<T>::destruct(&get<Is>())...); }

  standard_tuple_impl& operator=(standard_tuple_impl const& o) noexcept {
    swallow(LIFECYCLE_T<T>::assign(get<Is>(), o.get<Is>())...);
    return *this;
  }

  standard_tuple_impl& operator=(standard_tuple_impl&& o) noexcept {
    swallow(LIFECYCLE_T<T>::assign(get<Is>(), o.move_out<Is>())...);
    return *this;
  }

  bool operator==(standard_tuple_impl const& o) const { return (... && (get<Is>() == o.get<Is>())); }
  bool operator!=(standard_tuple_impl const& o) const { return (... || (get<Is>() != o.get<Is>())); }

  template <size_t IDX>
  auto& get() {
    static_assert(IDX < size(), "Overindexing a standard tuple");
    return data_.template get<IDX>();
  }

  template <size_t IDX>
  auto const& get() const {
    static_assert(IDX < size(), "Overindexing a standard tuple");
    return data_.template get<IDX>();
  }

  template <size_t IDX>
  constexpr static size_t offset() {
    static_assert(IDX < size(), "Overindexing a standard tuple");
    return offsetof(standard_tuple_impl, data_) + storage_t::offset_for(IDX);
  }

  constexpr static size_t size() { return sizeof...(T); }
};

template <template <typename> typename LIFECYCLE_PROXY, typename... T>
class generic_standard_tuple
    : public standard_tuple_impl<LIFECYCLE_PROXY, std::make_index_sequence<sizeof...(T)>, T...> {
  using standard_tuple_impl<LIFECYCLE_PROXY, std::make_index_sequence<sizeof...(T)>, T...>::standard_tuple_impl;
};

template <typename... T>
class standard_tuple : public generic_standard_tuple<standard_tuple_lifecycle_proxy, T...> {
  using generic_standard_tuple<standard_tuple_lifecycle_proxy, T...>::generic_standard_tuple;
};

}  // namespace tsar
