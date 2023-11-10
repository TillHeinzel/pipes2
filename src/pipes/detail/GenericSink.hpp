#pragma once

#include "Utility/HOF.hpp"

namespace pipes::detail
{
  template<class F>
  struct GenericSink
  {
    F f;

    template<class T>
      requires(std::invocable<F, T>)
    void push(T t)
    {
      f(std::move(t));
    }
  };
} // namespace pipes::detail
