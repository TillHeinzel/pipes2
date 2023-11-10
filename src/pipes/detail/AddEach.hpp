#pragma once

#include "Utility/HOF.hpp"
#include "Utility/ViewWrapper.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AddEach
  {
    R r;

    AddEach(R r) : r(std::move(r)), it(r.begin()) {}

    AddEach(AddEach&& other) : r(std::move(other.r)), it(r.begin()) {}

    AddEach(AddEach const& other) : r(other.r), it(r.begin()) {}

    decltype(r.begin()) it = r.begin();

    auto push(auto& sink, auto... ts)
    {
      if(it == r.end()) return;
      sink.push(std::move(ts)..., R::fwd(*it++));
    }
  };
} // namespace pipes::detail
