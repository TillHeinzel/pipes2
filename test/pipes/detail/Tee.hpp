#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class Side>
  struct Tee
  {
    Side v;

    template<class T>
      requires(SinkFor<Side, T>)
    auto push(SinkFor<T> auto& next, T&& t)
    {
      v.push(t);
      next.push(PIPES_FWD(t));
    }
  };

} // namespace pipes::detail
