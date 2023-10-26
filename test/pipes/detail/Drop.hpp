#pragma once

#include <concepts>

namespace pipes::detail
{
  auto fulfilledOnce(auto f)
  {
    return [f, fulfilled = false](auto&&... ts) mutable requires(
      std::invocable<decltype(f), decltype(ts)...>) {
      fulfilled = fulfilled || f(ts...);
      return fulfilled;
    };
  }

  auto invokedTimes(std::size_t count)
  {
    return [count, count_so_far = std::size_t(0)](auto const&...) mutable {
      ++count_so_far;
      return count_so_far > count;
    };
  }
} // namespace pipes::detail
