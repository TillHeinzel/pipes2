#pragma once

#include <numeric>
#include <ranges>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  auto reduce_f(auto f)
  {
    return [f](std::ranges::range auto const& r)
    {
      return std::reduce(r.begin(),
                         r.end(),
                         std::ranges::range_value_t<decltype(r)>{},
                         f);
    };
  }

  auto reduce_f(auto f, auto init)
  {
    return [f, init](std::ranges::range auto const& r)
    { return std::reduce(r.begin(), r.end(), init, f); };
  }
} // namespace pipes::detail
