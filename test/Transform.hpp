#pragma once

#include "PushBack.hpp"

namespace pipes
{
  template<class F>
  struct TransformNode
  {
    F f;
  };

  template<typename F>
  TransformNode<F> transform(F f)
    requires std::invocable<F, int> &&
             std::same_as<std::invoke_result_t<F, int>, int>
  {
    return {f};
  };

  template<class F>
  struct TransformSink
  {
    TransformNode<F> node;
    PushBackSink sink;

    void push(int i) { sink.push(node.f(i)); }
  };
}