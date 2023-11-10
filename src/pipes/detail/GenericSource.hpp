#pragma once

#include "Utility/HOF.hpp"

namespace pipes::detail
{
  template<class F>
  struct GenericSource
  {
    F f;

    void push(auto& sink)
    {
      f(sink);
    }
  };
} // namespace pipes::detail
