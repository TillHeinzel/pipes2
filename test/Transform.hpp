#pragma once

#include "impl.hpp"

namespace pipes
{
  template<class F>
  struct Transform
  {
    F f;

    void push(Sink<std::invoke_result_t<F, int>> auto& next, int i)
    {
      next.push(f(i));
    }
  };

  template<typename F>
  RawNodes<Transform<F>> transform(F f)
  {
    return RawNodes{Transform{f}};
  };
}
