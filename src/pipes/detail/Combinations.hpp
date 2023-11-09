#pragma once

#include "Utility/ViewWrapper.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct Combinations
  {
    R r;

    void push(SinkFor<value_t<R>, value_t<R>> auto& sink)
    {
      if(r.begin() == r.end()) return;

      for(auto it1 = r.begin(); std::next(it1) != r.end(); ++it1)
      {
        for(auto it2 = std::next(it1); it2 != r.end(); ++it2)
        {
          sink.push(*it1, *it2);
        }
      }
    }
  };
} // namespace pipes::detail
