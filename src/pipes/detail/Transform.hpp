#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F>
  struct Transform
  {
    F f;

    template<class... Ts>
    void push(SinkFor<std::invoke_result_t<F, Ts...>> auto& next, Ts&&... ts)
    {
      return next.push(f(PIPES_FWD(ts)...));
    }
  };
} // namespace pipes::detail
