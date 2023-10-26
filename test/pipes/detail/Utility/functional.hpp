#pragma once

namespace pipes::detail
{
  auto negate(auto f)
  {
    return [f](auto&&... ts) PIPES_RETURN(!f(PIPES_FWD(ts)...));
  }
} // namespace pipes::detail
