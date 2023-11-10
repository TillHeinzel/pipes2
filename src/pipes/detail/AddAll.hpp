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

    auto push(auto& sink, auto const&... ts)
    {
      for(const auto& x : r)
      {
        sink.push(ts..., x);
      }
    }
  };
} // namespace pipes::detail
