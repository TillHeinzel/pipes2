#pragma once

#include "Utility/HOF.hpp"

namespace pipes::detail
{
  template<class F>
  struct GenericSink
  {
    F f;

    auto push(auto&& t) PIPES_RETURN(f(PIPES_FWD(t)));
  };
} // namespace pipes::detail
