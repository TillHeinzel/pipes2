#pragma once

#include <concepts>

namespace pipes::detail
{
  auto fulfilledOnce(auto f)
  {
    return [f, fulfilled = false](auto const&... ts) mutable
    {
      fulfilled = fulfilled || f(ts...);
      return fulfilled;
    };
  }

  inline auto invokedTimes(std::size_t count)
  {
    return [count, count_so_far = std::size_t(0)](auto const&...) mutable
    {
      ++count_so_far;
      return count_so_far > count;
    };
  }
} // namespace pipes::detail
