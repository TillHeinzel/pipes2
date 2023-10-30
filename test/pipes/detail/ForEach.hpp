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
      for(auto const& t : v) { sink.push(t); }
    }
  };

  template<std::ranges::range... Rs>
  struct MultiForEach
  {
    std::tuple<Rs const&...> vs;

    void push(SinkFor<std::tuple<std::ranges::range_value_t<Rs>...>> auto& sink)
    {
      auto doPush = [&sink](auto&&... ts) {
        sink.push(std::tuple<std::ranges::range_value_t<Rs>...>{PIPES_FWD(ts)...});
      };

      parallelIterate(vs, doPush);
    }
  };
} // namespace pipes::detail
