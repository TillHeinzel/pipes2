#pragma once

#include "impl.hpp"

namespace pipes
{
  template<class F>
  struct Transform
  {
    F f;

    template<class T>
    void push(Sink<std::invoke_result_t<F, T>> auto& next, T t)
    {
      next.push(f(t));
    }
  };

  template<typename F>
  RawNodes<Transform<F>> transform(F f)
  {
    return RawNodes{Transform{f}};
  };
}
