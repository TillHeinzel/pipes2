#pragma once

#include "../GenericImplementation/impl.hpp"

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

  namespace api
  {
    template<typename F>
    auto filter(F f) PIPES_RETURN(pipe(Filter{f}));
  } // namespace api
} // namespace pipes::detail