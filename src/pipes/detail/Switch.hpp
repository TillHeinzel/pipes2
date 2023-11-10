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

    void push(auto& next, auto... ts)
    {
      if(f(ts...))
      {
        r.push(std::move(ts)...);
      }
      else
      {
        next.push(std::move(ts)...);
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
