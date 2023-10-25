#pragma once

namespace pipes::detail
{
  template<typename S, class... T>
  concept SinkFor = requires(S s, T... t) { s.push(t...); };
} // namespace pipes::detail
