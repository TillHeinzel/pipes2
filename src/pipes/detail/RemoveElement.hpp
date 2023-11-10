#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class Side>
  struct RemoveElement
  {
    Side side;

    void push(auto& next, auto t, auto... ts)
    {
      side.push(std::move(t));
      next.push(std::move(ts)...);
    }
  };
} // namespace pipes::detail
