#pragma once

#include "impl.hpp"

namespace pipes
{
  template<class F>
  struct Filter
  {
    F f;

    void push(Sink<int> auto& next, int i)
    {
      if(f(i)) next.push(i);
    }
  };

  template<typename F>
  RawNodes<Filter<F>> filter(F f)
  {
    return RawNodes{Filter{f}};
  }
} // namespace pipes