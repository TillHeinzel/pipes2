#pragma once

#include <ranges>
#include <vector>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  struct Flatten
  {
    template<std::ranges::range R>
    void push(auto& next, R v)
    {
      for(auto& t : v) next.push(std::move(t));
    }
  };
} // namespace pipes::detail
