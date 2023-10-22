#pragma once

#include "impl.hpp"

namespace pipes
{
  template<class F>
  struct FilterOp
  {
    constexpr static bool isOp = true;

    F f;

    void push(Sink<int> auto& next, int i)
    {
      if(f(i)) next.push(i);
    }
  };

  template<typename F>
  RawNodes<FilterOp<F>> filter(F f)
    requires std::invocable<F, int> &&
             std::same_as<std::invoke_result_t<F, int>, bool>
  {
    return {FilterOp{f}};
  }


} // namespace pipes