#pragma once

#include "Utility/ViewWrapper.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AdjacentSource
  {
    R r;

    auto push(SinkFor<value_t<R>, value_t<R>> auto& sink)
    {
      if(r.begin() == r.end()) return;

      auto it1 = r.begin();
      auto it2 = std::next(it1);

      while(it2 != r.end())
      {
        sink.push(*it1++, *it2++);
      }
    };
  };
} // namespace pipes::detail
