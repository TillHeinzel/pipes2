#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class Side>
  struct RemoveElement
  {
    Side side;

    template<class T, class... Ts>
      requires(SinkFor<Side, T>)
    void push(SinkFor<Ts...> auto& next, T&& t, Ts&&... ts)
    {
      side.push(PIPES_FWD(t));
      next.push(PIPES_FWD(ts)...);
    }
  };
} // namespace pipes::detail
