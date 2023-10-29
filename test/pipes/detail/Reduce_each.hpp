#pragma once

#include <numeric>
#include <ranges>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  auto reduce_f(auto f)
  {
    return [f](auto&& r)
      PIPES_RETURN(std::reduce(r.begin(),
                               r.end(),
                               std::ranges::range_value_t<decltype(r)>{},
                               f));
  }

  auto reduce_f(auto f, auto init)
  {
    return [f, init](auto&& r)
      PIPES_RETURN(std::reduce(r.begin(), r.end(), init, f));
  }
} // namespace pipes::detail
