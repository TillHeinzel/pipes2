#pragma once

#include <tuple>

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
      std::apply([&t](auto&... ns) { (ns.push(t), ...); }, nexts);
    }
  };
} // namespace pipes::detail
