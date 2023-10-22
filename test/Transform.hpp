#pragma once

#include "impl.hpp"

namespace pipes
{
  template<class F>
  struct TransformOp
  {
    constexpr static bool isOp = true;

    F f;

    void push(Sink<std::invoke_result_t<F, int>> auto& next, int i)
    {
      next.push(f(i));
    }
  };

  template<typename F>
  RawNode<TransformOp<F>> transform(F f)
    requires std::invocable<F, int> &&
             std::same_as<std::invoke_result_t<F, int>, int>
  {
    return {f};
  };
}
