#pragma once

#include <concepts>

#include "../GenericImplementation/impl.hpp"
#include "../Utility/functional.hpp"

#include "Filter.hpp"

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

  namespace api
  {
    auto drop_until(auto f) { return filter(fulfilledOnce(f)); }

    auto drop_while(auto f) { return drop_until(negate(f)); }

    auto drop(std::size_t count)
    {
      auto invocedCountTimes = [count, count_so_far = 0](auto const&...) mutable {
        ++count_so_far;
        return count_so_far > count;
      };

      return drop_until(invocedCountTimes);
    }
  } // namespace api
} // namespace pipes::detail
