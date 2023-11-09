#pragma once

#include "Utility/HOF.hpp"


namespace pipes::detail
{
  template<class F>
  struct GenericSource
  {
    F f;

    auto push(auto& sink) PIPES_RETURN(f(sink));
  };
} // namespace pipes::detail
