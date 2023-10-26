#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F>
  struct Filter
  {
    F f;

    template<class... Ts>
      requires(std::invocable<F, Ts...>)
    auto push(SinkFor<Ts...> auto& next, Ts&&... ts)
    {
      if(f(ts...)) next.push(PIPES_FWD(ts)...);
    }
  };
} // namespace pipes::detail
