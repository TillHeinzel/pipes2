#pragma once

namespace pipes::detail
{
  inline auto stride_f(std::size_t step, std::size_t offset)
  {
    return [step, offset, count = 0](auto const&...) mutable {
      const auto effectiveCount = count >= offset ? (count - offset) : 1;
      ++count;
      return effectiveCount % step == 0;
    };
  }
} // namespace pipes::detail
