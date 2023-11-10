#pragma once

#include "Utility/ViewWrapper.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct ForEach
  {
    R r;

    void push(auto& sink)
    {
      for(decltype(auto) t : r)
      {
        sink.push(R::fwd(t));
      }
    }
  };
} // namespace pipes::detail
