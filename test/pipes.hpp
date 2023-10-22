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

#include "PushBack.hpp"

namespace pipes
{
  template<class... Ops>
  auto operator>>=(RawNodes<Ops...> n, std::vector<int>& v)
  {
    return n >>= PushBackSink{v};
  }

  void operator>>=(std::vector<int> const& source, Sink<int> auto sink)
  {
    for(const int i : source) { sink.push(i); }
  }

  template<class... Ops>
  auto operator>>=(std::vector<int> const& source, RawNodes<Ops...> n) 
  {
    return Source{source, n};
  }

  template<class... Ops>
  void operator>>=(Source<Ops...> source, Sink<int> auto sink)
  {
    return source.root >>= (source.ops >>= sink);
  }

  template<class... Ops>
  void operator>>=(Source<Ops...> source, std::vector<int>& v)
  {
    return source >>= PushBackSink{v};
  }
} // namespace pipes


#include "Filter.hpp"
#include "Transform.hpp"