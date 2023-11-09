#pragma once

#include <concepts>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F, class R>
  struct TakeIf
  {
    F f;
    R r;
    
    template<class... Ts>
      requires(SinkFor<R, Ts...> && std::invocable<F, Ts...>)
    void push(SinkFor<Ts...> auto& next, Ts&&... ts)
    {
      if(f(ts...))
      {
        r.push(PIPES_FWD(ts)...);
      }
      else
      {
        next.push(PIPES_FWD(ts)...);
      }
    }
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<class F, class... Pieces>
  struct CaseSection
  {
    F f;
    Section<Pieces...> pipe;
  };

  template<class F, class S>
  struct CaseSink
  {
    F f;
    S sink;
  };
} // namespace pipes::detail
