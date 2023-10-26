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
  template<class... Ts, class F>
  auto transform(std::tuple<Ts...>& ts, F const& f) 
  {
    return std::apply([&f](auto&&... ts) { return std::tuple{f(ts)...}; },
                      PIPES_FWD(ts));
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class Tuple_>
  auto for_each(auto&& f, Tuple_&& tup)
    PIPES_RETURN(std::apply([&f](auto&&... ts) { (f(ts), ...); },
                            PIPES_FWD(tup)))

} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Its, class... Ends, std::size_t... Is>
  bool any_end_impl(std::tuple<Its...> const& its,
                    std ::tuple<Ends...> const& ends,
                    std::index_sequence<Is...>)
  {
    return ((std::get<Is>(its) == std::get<Is>(ends)) || ...);
  }

  template<class... Its, class... Ends>
  bool any_end(std::tuple<Its...> its, std::tuple<Ends...> ends)
  {
    return any_end_impl(its, ends, std::index_sequence_for<Its...>{});
  }
} // namespace pipes::detail