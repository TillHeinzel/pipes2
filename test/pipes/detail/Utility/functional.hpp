#pragma once

namespace pipes::detail
{
  auto negate(auto f)
  {
    return [f](auto&&... ts) mutable PIPES_RETURN(!f(PIPES_FWD(ts)...));
  }
} // namespace pipes::detail
