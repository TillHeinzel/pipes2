#pragma once

#include "../GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F>
  struct Filter
  {
    F f;

    template<class T>
    void push(SinkFor<T> auto& next, T const& t)
    {
      if(f(t)) next.push(t);
    }
  };

  namespace api
  {
    template<typename F>
    auto filter(F f) PIPES_RETURN(RawNodes{Filter{f}});
  } // namespace api
} // namespace pipes::detail