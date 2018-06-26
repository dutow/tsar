
#include <boost/hana.hpp>
#include <type_traits>

#include "tsar/standard_tuple.hpp"

namespace tsar {

struct context_aware_dummy {};

template <typename TUPLE_T, size_t IDX, size_t OFFSET>
struct context_aware_reference {
  static constexpr const size_t offset = OFFSET;
  static constexpr const size_t idx = IDX;
  using tuple_t = TUPLE_T;

  static tuple_t& container(void* self_addr) {
    return *reinterpret_cast<tuple_t*>(reinterpret_cast<char*>(self_addr) - offset);
  }

  static tuple_t const& container(const void* self_addr) {
    return *reinterpret_cast<const tuple_t*>(reinterpret_cast<const char*>(self_addr) - offset);
  }
};

template <typename T>
struct context_aware_helper {
  template <size_t IDX>
  auto& get() {
    return T::container(this).template get<IDX>();
  }

  template <size_t IDX>
  auto const& get() const {
    return T::container(this).template get<IDX>();
  }
};

template <template <typename> typename... T>
class context_aware_tuple;

template <typename TUPLE_T, typename I, template <typename> typename... T>
class context_aware_tuple_helper;

template <typename TUPLE_T, size_t... Is, template <typename> typename... T>
class context_aware_tuple_helper<TUPLE_T, std::index_sequence<Is...>, T...> {
 private:
  using real_type = TUPLE_T;

  using dummy_tuple_t = standard_storage<T<context_aware_dummy>...>;

  template <template <typename> typename TT, size_t IDX>
  using real_type_for =
      TT<context_aware_reference<context_aware_tuple<T...>, IDX, dummy_tuple_t::offset_for(IDX)>>;

  template <typename TT>
  struct lifecycle_proxy {
    static int construct(void* addr) {
      new (addr) TT();
      return 0;
    }

    template <typename... TA>
    static int construct(void* addr, TA&&... args) {
      new (addr) TT(std::forward<TA>(args)...);
      return 0;
    }

    template <typename... TA>
    static int construct(void* addr, TA const&... args) {
      new (addr) TT(args...);
      return 0;
    }

    static int assign(TT& to, TT&& from) {
      to = std::forward<TT>(from);
      return 0;
    }

    static int assign(TT& to, TT const& from) {
      to = from;
      return 0;
    }

    static int destruct(TT* object) {
      object->~TT();
      return 0;
    }
  };

  template <template <typename> typename TT, size_t IDX>
  struct wrapper_type_for : public real_type_for<TT, IDX> {
    wrapper_type_for& operator=(wrapper_type_for const&) = default;
    wrapper_type_for& operator=(wrapper_type_for&&) = default;

   public:
   private:
    wrapper_type_for() : real_type_for<TT, IDX>() {}

    template <typename... TTT>
    wrapper_type_for(std::tuple<TTT...>&& args)
        : real_type_for<TT, IDX>(std::make_from_tuple<real_type_for<TT, IDX>>(args)) {}

    // a simple wrapper type to disable outside copies / moves from this type
    wrapper_type_for(wrapper_type_for const&) = default;
    wrapper_type_for(wrapper_type_for&&) = default;

    ~wrapper_type_for() {}

    template <typename TX>
    friend struct lifecycle_proxy;
  };

  using type_list = decltype(boost::hana::make_tuple(boost::hana::type_c<real_type_for<T, Is>>...));

 public:
  using type = generic_standard_tuple<lifecycle_proxy, wrapper_type_for<T, Is>...>;

  static_assert(sizeof(dummy_tuple_t) == sizeof(type),
                "Member types have the same size regardless of the context parameter");
};

template <template <typename> typename... T>
class context_aware_tuple
    : public context_aware_tuple_helper<context_aware_tuple<T...>, std::make_index_sequence<sizeof...(T)>, T...>::type {
 public:
  using base_t = typename context_aware_tuple_helper<context_aware_tuple<T...>, std::make_index_sequence<sizeof...(T)>,
                                                     T...>::type;

  using base_t::base_t;
};
}  // namespace tsar
