#pragma once

#include <ranges>
#include <vector>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  struct Flatten
  {
    template<std::ranges::range R>
    void push(SinkFor<std::ranges::range_value_t<R>> auto& next, R const& v)
    {
      for(auto const& t : v) next.push(t);
    }
  };
} // namespace pipes::detail
