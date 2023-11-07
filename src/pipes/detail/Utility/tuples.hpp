#pragma once

#include <ranges>
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
  template<class It, class End>
  struct IteratorPair
  {
    It it;
    End end;
  };

  template<std::ranges::range... Rs>
  auto getIteratorPairs(std::tuple<Rs...>& rs)
  {
    constexpr auto getIteratorPair = [](auto const& v) {
      return IteratorPair{v.begin(), v.end()};
    };

    return transform(rs, getIteratorPair);
  }

  template<class... Its, class... Ends>
  void parallelIterate_impl(auto f, IteratorPair<Its, Ends>... itss)
  {
    while(!((itss.it == itss.end) || ...))
    {
      f(*itss.it...);
      (++itss.it, ...);
    }
  }

  template<class... Its, class... Ends>
  void parallelIterate(std::tuple<IteratorPair<Its, Ends>...> itss, auto f)
  {
    auto ff = [f](auto&&... itss)
    { parallelIterate_impl(f, PIPES_FWD(itss)...); };
    std::apply(ff, itss);
  }

  template<std::ranges::range... Rs>
  void parallelIterate(std::tuple<Rs...>& ranges, auto f)
  {
    parallelIterate(getIteratorPairs(ranges), f);
  }

} // namespace pipes::detail

namespace pipes::detail
{
  template<class F, class... Ts>
  void callFirstSuccess(std::tuple<Ts...> cases, F f)
  {
    auto ff = [f](auto... ts)
    {
      // short-circuits. after success, no more calls to f
      return (f(ts) || ...);
    };

    std::apply(ff, cases);
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class Tup1, class Tup2, std::size_t... Is>
  auto tuple_zip_impl(Tup1 tup1, Tup2 tup2, std::index_sequence<Is...>)
  {
    return std::make_tuple(
      std::tuple{std::get<Is>(tup1), std::get<Is>(tup2)}...);
  }

  template<class Tup1, class Tup2>
  auto tuple_zip(Tup1 tup1, Tup2 tup2)
  {
    static_assert(std::tuple_size_v<Tup1> == std::tuple_size_v<Tup2>);

    return tuple_zip_impl(tup1,
                          tup2,
                          std::make_index_sequence<std::tuple_size_v<Tup1>>());
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class X>
  struct is_tuple : std::false_type
  {
  };

  template<class... Ts>
  struct is_tuple<std::tuple<Ts...>> : std::true_type
  {
  };

  template<class Tup>
  constexpr static bool is_tuple_v = is_tuple<Tup>::value;

  template<class X>
  struct is_pair : std::false_type
  {
  };

  template<class T1, class T2>
  struct is_pair<std::pair<T1, T2>> : std::true_type
  {
  };

  template<class Tup>
  constexpr static bool is_pair_v = is_pair<Tup>::value;

  template<class Tup>
  concept Tuple_like = is_tuple_v<Tup> || is_pair_v<Tup>;

  template<class Tup>
  concept Pair_like =
    is_pair_v<Tup> || (is_tuple_v<Tup> && std::tuple_size_v<Tup> == 2);
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts>
  bool any_of(std::tuple<Ts...> ts)
  {
    return std::apply([](auto... ts) { return (ts || ...); }, ts);
  }

  template<class F, class... Ts>
  bool any_of(std::tuple<Ts...> ts, F f)
  {
    return std::apply([](auto... ts) { return (ts || ...); }, transform(ts, f));
  }
} // namespace pipes::detail
