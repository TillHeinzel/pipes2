#pragma once

#include <concepts>
#include <tuple>
#include <vector>

#include "forUI.hpp"

#include "SpecificPipes/ForEach.hpp"
#include "SpecificPipes/PushBack.hpp"

namespace pipes::detail
{
  auto operator>>=(auto source, auto sink) PIPES_FWD(append(source, sink));

  template<class T>
  auto operator>>=(std::vector<T> const& v, ValidReceiverFor<T> auto n)
    PIPES_FWD(api::forEach(v) >>= n);

  template<class T, class... Ops>
  auto operator>>=(Source<Ops...> source, std::vector<T>& v)
    PIPES_FWD(source >>= api::push_back(v));

  template<class T, class... Ops>
  auto operator>>=(RawNodes<Ops...> n, std::vector<T>& v)
  {
    return n >>= api::push_back(v);
  }
} // namespace pipes::detail
