#pragma once

#include <concepts>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F, class IfTrue, class IfFalse>
  struct Partition
  {
    F f;
    IfTrue ifTrue;
    IfFalse ifFalse;

    template<class... Ts>
      requires(std::invocable<F, Ts...> && SinkFor<IfTrue, Ts...> &&
               SinkFor<IfFalse, Ts...>)
    void push(Ts&&... ts)
    {
      if(f(ts...)) { ifTrue.push(PIPES_FWD(ts)...); }
      else { ifFalse.push(PIPES_FWD(ts)...); }
    }
  };
} // namespace pipes::detail
