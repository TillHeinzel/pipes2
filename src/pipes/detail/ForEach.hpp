#pragma once

#include "Utility/ViewWrapper.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct ForEach
  {
    R r;

    void push(SinkFor<value_t<R>> auto& sink)
    {
      for(auto const& t : r)
      {
        sink.push(t);
      }
    }
  };
} // namespace pipes::detail
