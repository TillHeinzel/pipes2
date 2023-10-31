#pragma once

#include <ranges>

#include "Utility/tuples.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<std::ranges::range R>
  struct ForEach
  {
    R const& v;

    void push(SinkFor<std::ranges::range_value_t<R>> auto& sink)
    {
      for(auto const& t : v)
      {
        sink.push(t);
      }
    }
  };

  template<std::ranges::range... Rs>
  struct MultiForEach
  {
    std::tuple<Rs const&...> vs;

    void push(SinkFor<std::tuple<std::ranges::range_value_t<Rs>...>> auto& sink)
    {
      auto doPush = [&sink](auto&&... ts) {
        sink.push(
          std::tuple<std::ranges::range_value_t<Rs>...>{PIPES_FWD(ts)...});
      };

      parallelIterate(vs, doPush);
    }
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<class F>
  struct GenericSource
  {
    F f;

    auto push(auto& sink) PIPES_RETURN(f(sink));
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<class R>
  struct AdjacentSource
  {
    R const& r;

    auto push(auto& sink)
    {
      if(r.begin() == r.end()) return;

      auto it1 = r.begin();
      auto it2 = std::next(it1);

      while(it2 != r.end())
      {
        sink.push(*it1++, *it2++);
      }
    };
  };
} // namespace pipes::detail
