#pragma once

#include <concepts>
#include <tuple>
#include <vector>

#include "impl.hpp"

namespace pipes
{
  template<class... Op1, class... Op2>
  auto operator>>=(RawNodes<Op1...> n1, RawNodes<Op2...> n2)
  {
    return addBefore(n2, n1);
  }

  template<class... Ops>
  auto operator>>=(RawNodes<Ops...> n, OpenSink auto s)
  {
    return addBefore(s, n);
  }
} // namespace pipes

namespace pipes
{
  template<class... Ops>
  void operator>>=(Source<Ops...> source, Sink<int> auto sink)
  {
    finish(source, sink);
  }

  template<class... Ops>
  void operator>>=(Source<Ops...> source, Sink<std::string> auto sink)
  {
    finish(source, sink);
  }

  template<class... Ops1, class... Ops2>
  auto operator>>=(Source<Ops1...> source, RawNodes<Ops2...> ops)
  {
    return addBefore(ops, source);
  }
} // namespace pipes

#include "PushBack.hpp"
#include "ForEach.hpp"

namespace pipes
{
  template<class... Ops>
  auto operator>>=(std::vector<int> const& v, RawNodes<Ops...> n)
  {
    return forEach(v) >>= n;
  }

  template<class T>
  void operator>>=(std::vector<T> const& source, Sink<T> auto sink)
  {
    forEach(source) >>= sink;
  }

  template<class... Ops>
  void operator>>=(Source<Ops...> source, std::vector<int>& v)
  {
    return source >>= push_back(v);
  }

  template<class T, class... Ops>
  auto operator>>=(RawNodes<Ops...> n, std::vector<T>& v)
  {
    return n >>= push_back(v);
  }
} // namespace pipes

#include "Filter.hpp"
#include "Transform.hpp"