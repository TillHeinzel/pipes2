#pragma once

#include "Utility/tuples.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class T>
  struct ForEach
  {
    std::vector<T> const& v;

    void push(SinkFor<T> auto sink)
    {
      for(T const& t : v) { sink.push(t); }
    }
  };

  template<class... Ts>
  struct MultiForEach
  {
    std::tuple<std::vector<Ts> const&...> vs;

    void push(SinkFor<std::tuple<Ts...>> auto sink)
    {
      constexpr auto begin = [](auto const& v) { return v.begin(); };
      constexpr auto end = [](auto const& v) { return v.end(); };
      constexpr auto dereference = [](auto it) { return *it; };
      constexpr auto progress = [](auto& it) { return ++it; };

      auto its = transform(vs, begin);
      auto ends = transform(vs, end);

      while(!any_end(its, ends))
      {
        sink.push(std::tuple<Ts...>{transform(its, dereference)});

        transform(its, progress);
      }
    }
  };
} // namespace pipes::detail
