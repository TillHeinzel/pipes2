#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F>
  struct Transform
  {
    F f;

    void push(auto& next, auto... ts) { return next.push(f(std::move(ts)...)); }
  };
} // namespace pipes::detail
