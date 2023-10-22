#pragma once

#include "PushBack.hpp"

namespace pipes
{
  template<class F>
  struct FilterNode
  {
    F f;
  };

  template<class F>
  struct FilterSink
  {
    FilterNode<F> node;
    PushBackSink sink;

    void push(int i)
    {
      if(node.f(i)) sink.push(i);
    }
  };

  template<typename F>
  FilterNode<F> filter(F f)
    requires std::invocable<F, int> &&
             std::same_as<std::invoke_result_t<F, int>, bool>
  {
    return {f};
  }
} // namespace pipes