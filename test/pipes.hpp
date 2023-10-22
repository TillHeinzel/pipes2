#pragma once

#include <concepts>
#include <vector>

#include "impl.hpp"

#include "Filter.hpp"
#include "PushBack.hpp"
#include "Transform.hpp"

namespace pipes
{
  template<class Op>
  auto operator>>=(RawNode<Op> n, OpenSink auto s) -> decltype(Node{n.op, s})
  {
    return Node{n.op, s};
  }

  template<class Op1, class Op2>
  auto operator>>=(RawNode<Op1> n1, RawNode<Op2> n2)
  {
    return std::tuple{n1, n2};
  }

  template<class Op1, class Op2, class Op3>
  auto operator>>=(RawNode<Op1> n1, std::tuple<Op2, Op3> n2)
  {
    return std::tuple{n1, std::get<0>(n2), std::get<1>(n2)};
  }

  template<class Op1, class Op2>
  auto operator>>=(std::tuple<Op1, Op2> chain, OpenSink auto s)
  {
    return std::get<0>(chain) >>= std::get<1>(chain) >>= s;
  }

  template<class Op1, class Op2, class Op3>
  auto operator>>=(std::tuple<Op1, Op2, Op3> chain, OpenSink auto s)
  {
    return std::get<0>(chain) >>= std::get<1>(chain) >>= std::get<2>(chain) >>= s;
  }
} // namespace pipes

namespace pipes
{
  template<class Op>
  Node<Op, PushBackSink> operator>>=(RawNode<Op> n, std::vector<int>& v)
  {
    return n >>= PushBackSink{v};
  }

  template<class Op1, class Op2>
  auto operator>>=(std::tuple<Op1, Op2> chain, std::vector<int>& v)
  {
    return chain >>= PushBackSink{v};
  }

  template<class Op1, class Op2, class Op3>
  auto operator>>=(std::tuple<Op1, Op2, Op3> chain, std::vector<int>& v)
  {
    return chain >>= PushBackSink{v};
  }

  void operator>>=(std::vector<int> const& source, Sink<int> auto sink)
  {
    for(const int i : source) { sink.push(i); }
  }
} // namespace pipes
