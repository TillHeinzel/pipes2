#pragma once

#include "Utility/ViewWrapper.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct MixIn
  {
    R r;
    std::size_t skips;

    MixIn(R r, std::size_t skips) : r(std::move(r)), skips(skips), it(r.begin()) {}

    MixIn(MixIn&& other) :
      r(std::move(other.r)),
      skips(other.skips),
      it(r.begin())
    {
    }

    MixIn(MixIn const& other) : r(other.r), skips(other.skips), it(r.begin()) {}

    decltype(r.begin()) it = r.begin();
    std::size_t skipsTaken = 0;

    void push(auto& next, auto t)
    {
      if(it == r.end()) return;

      next.push(std::move(t));
      skipsTaken++;

      if(skipsTaken == skips)
      {
        next.push(R::fwd(*it++));
        skipsTaken = 0;
      }
    }
  };
} // namespace pipes::detail
