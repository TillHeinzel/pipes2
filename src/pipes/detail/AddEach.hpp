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

    AddEach(R r) : r(r), it(r.begin()) {}

    AddEach(AddEach&& other) : r(std::move(other.r)), it(r.begin()) {}

    AddEach(AddEach const& other) : r(other.r), it(r.begin()) {}

    decltype(r.begin()) it = r.begin();

    template<class... Ts>
    auto push(SinkFor<Ts..., value_t<R>> auto& sink, Ts&&... ts)
    {
      if(it == r.end()) return;
      sink.push(PIPES_FWD(ts)..., *it++);
    }
  };
} // namespace pipes::detail
