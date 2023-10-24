#pragma once

#include <vector>

#include "../GenericImplementation/impl.hpp"

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

  template<class... Ts>
  struct Zip
  {
    using OutputType = std::tuple<Ts...>;

    std::tuple<std::vector<Ts> const&...> vs;

    void push(SinkFor<OutputType> auto sink)
    {
      constexpr auto begin = [](auto const& v) { return v.begin(); };
      constexpr auto end = [](auto const& v) { return v.end(); };
      constexpr auto dereference = [](auto it) { return *it; };
      constexpr auto progress = [](auto& it) { return ++it; };

      auto its = transform(vs, begin);
      auto ends = transform(vs, end);

      while(!any_end(its, ends))
      {
        sink.push(OutputType{transform(its, dereference)});

        transform(its, progress);
      }
    }
  };

  namespace api
  {
    template<class... Ts>
    auto zip(std::vector<Ts> const&... vs)
      PIPES_RETURN(Source{Zip{std::tie(vs...)}});
  } // namespace api
} // namespace pipes::detail