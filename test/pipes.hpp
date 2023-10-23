#pragma once

#include <concepts>
#include <tuple>
#include <vector>

#include "impl.hpp"

namespace pipes
{
  template<class... Op1, class... Op2>
  auto operator>>=(RawNodes<Op1...> n1, RawNodes<Op2...> n2)
    PIPES_FWD(append(n1, n2));

  template<class... Ops>
  auto operator>>=(RawNodes<Ops...> n, OpenSink auto s)
    PIPES_FWD(append(n, s));
} // namespace pipes

namespace pipes
{
  template<class... Ops>
  auto operator>>=(Source<Ops...> source, OpenSink auto sink)
    PIPES_FWD(append(source, sink));

  template<class... Ops1, class... Ops2>
  auto operator>>=(Source<Ops1...> source, RawNodes<Ops2...> ops)
    PIPES_FWD(append(source, ops));

  
  //auto operator>>=(auto source, auto sink)
  //  PIPES_FWD(append(source, sink));
} // namespace pipes

#include "ForEach.hpp"
#include "PushBack.hpp"

namespace pipes
{
  template<class T>
  auto operator>>=(std::vector<T> const& v, ValidReceiverFor<T> auto n)
    PIPES_FWD(forEach(v) >>= n);

  template<class T, class... Ops>
  auto operator>>=(Source<Ops...> source, std::vector<T>& v)
    PIPES_FWD(source >>= push_back(v));

  template<class T, class... Ops>
  auto operator>>=(RawNodes<Ops...> n, std::vector<T>& v)
    PIPES_FWD(n >>= push_back(v));
} // namespace pipes

#include "Filter.hpp"
#include "Transform.hpp"