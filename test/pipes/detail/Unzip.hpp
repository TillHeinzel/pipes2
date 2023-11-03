#pragma once

#include "Utility/tuples.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  constexpr auto doPush = [](auto& next, auto&&... ts)
  {
    next.push(PIPES_FWD(ts)...); //
  };

  template<class... Nexts>
  struct Unzip
  {
    std::tuple<Nexts...> nexts;

    template<class... Ts>
      requires(SinkFor<Nexts, Ts> && ...)
    void push(Ts&&... ts)
    {
      auto nextsAndTs = tuple_zip(nexts, std::forward_as_tuple<Ts...>(ts...));

      for_each([](auto nextAndT) { std::apply(doPush, nextAndT); }, nextsAndTs);
    }
  };

} // namespace pipes::detail
