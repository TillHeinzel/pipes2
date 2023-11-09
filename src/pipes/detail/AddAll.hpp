#pragma once

#include "Utility/HOF.hpp"
#include "Utility/ViewWrapper.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AddAll
  {
    R r;

    template<class... Ts>
    auto push(SinkFor<Ts..., value_t<R>> auto& sink, Ts&&... ts)
    {
      for(const auto& x : r)
      {
        sink.push(PIPES_FWD(ts)..., x);
      }
    }
  };
} // namespace pipes::detail
