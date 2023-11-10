#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  struct TupleFlatten
  {
    void push(auto& next, auto... ts) { (next.push(ts), ...); }
  };
} // namespace pipes::detail
