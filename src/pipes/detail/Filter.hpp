#pragma once

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class F>
  struct Filter
  {
    F f;

    auto push(auto& next, auto... ts)
    {
      if(f(ts...)) next.push(std::move(ts)...);
    }
  };
} // namespace pipes::detail
