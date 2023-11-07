#pragma once

#include <tuple>

#include "Utility/tuples.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class... Nexts>
  struct Fork
  {
    std::tuple<Nexts...> nexts;

    template<class T>
      requires(SinkFor<Nexts, T> && ...)
    void push(T const& t)
    {
      for_each([&t](auto& n) { n.push(t); }, nexts);
    }
  };
} // namespace pipes::detail
