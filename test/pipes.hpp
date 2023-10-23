#pragma once

#include <concepts>
#include <tuple>
#include <vector>

#include "forUI.hpp"

namespace pipes::detail
{
  auto operator>>=(auto source, auto sink)
    PIPES_FWD(append(source, sink));
} // namespace pipes

#include "ForEach.hpp"
#include "PushBack.hpp"

namespace pipes
{
  using namespace detail::api;
}

namespace pipes::detail
{
  template<class T>
  auto operator>>=(std::vector<T> const& v, ValidReceiverFor<T> auto n)
    PIPES_FWD(forEach(v) >>= n);

  template<class T, class... Ops>
  auto operator>>=(Source<Ops...> source, std::vector<T>& v)
    PIPES_FWD(source >>= push_back(v));

  template<class T, class... Ops>
  auto operator>>=(RawNodes<Ops...> n, std::vector<T>& v)
  {
    return n >>= push_back(v);
  }
} // namespace pipes


#include "Filter.hpp"
#include "Transform.hpp"

#include "Discard.hpp"
#include "Zip.hpp"
