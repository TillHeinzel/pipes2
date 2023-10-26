#pragma once

#include <tuple>

namespace pipes::detail
{
  template<typename T, size_t... I>
  auto reverse_impl(T&& t, std::index_sequence<I...>)
  {
    return std::make_tuple(
      std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
  }

  template<typename T>
  auto reverse(T&& t)
  {
    return reverse_impl(
      std::forward<T>(t),
      std::make_index_sequence<
        std::tuple_size<std::remove_reference_t<T>>::value>());
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts, class F, std::size_t... Is>
  auto tuple_transform_impl(std::tuple<Ts...>& t,
                            F const& f,
                            std::index_sequence<Is...>)
  {
    return std::tuple{f(std::get<Is>(t))...};
  }

  template<class... Ts, class F>
  auto transform(std::tuple<Ts...>& t, F const& f)
  {
    return tuple_transform_impl(t, f, std::index_sequence_for<Ts...>{});
  }

} // namespace pipes::detail

namespace pipes::detail
{
  template<class Tuple_>
  auto for_each(auto&& f, Tuple_&& tup)
    PIPES_RETURN(std::apply([&f](auto&&... ts) { (f(ts), ...); },
                            PIPES_FWD(tup)))

} // namespace pipes::detail