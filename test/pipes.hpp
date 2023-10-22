#pragma once

#include <concepts>
#include <tuple>
#include <vector>

#include "impl.hpp"

#include "Filter.hpp"
#include "PushBack.hpp"
#include "Transform.hpp"

namespace pipes
{
  template<class Op>
  auto addBefore(OpenSink auto s, Op op)
  {
    return Node{op, s};
  }

  template<class Op, class... Ops>
  auto addBefore(OpenSink auto s, Op n, Ops... nodes)
  {
    return addBefore(addBefore(s, n), nodes...);
  }

  template<class... Ops>
  auto addBefore(OpenSink auto s, std::tuple<Ops...> ops)
  {
    auto f = [&](auto... n) { return addBefore(s, n...); };
    return std::apply(f, ops);
  }

  //template<class... Ops>
  //auto addBefore(OpenSink auto s, RawNodes<Ops...> ops)
  //{
  //  auto f = [&](auto... n) { return addBefore(s, n...); };
  //  return std::apply(f, ops);
  //}

  template<class Op1, class Op2>
  auto addBefore(Op2 op2, Op1 op1)
  {
    return std::tuple{op2, op1};
  }

  template<class Op1, class... Ops>
  auto addBefore(std::tuple<Ops...> ops, Op1 op1)
  {
    return std::tuple_cat(ops, std::tuple{
    op1});
  }

} // namespace pipes

namespace pipes
{
  template<class Op>
  auto operator>>=(RawNodes<Op> n, OpenSink auto s)
  {
    return addBefore(s, std::get<0>(n));
  }

  template<class Op1, class Op2>
  auto operator>>=(RawNodes<Op1> n1, RawNodes<Op2> n2)
  {
    return addBefore(std::get<0>(n2), std::get<0>(n1));
  }

  template<class Op1, class Op2, class Op3>
  auto operator>>=(RawNodes<Op1> n1, std::tuple<Op2, Op3> n2)
  {
    return addBefore(n2, std::get<0>(n1));
  }

  template<class... Ops>
  auto operator>>=(std::tuple<Ops...> chain, OpenSink auto s)
  {
    return addBefore(s, chain);
  }
} // namespace pipes

namespace pipes
{
  template<class Op>
  Node<Op, PushBackSink> operator>>=(RawNodes<Op> n, std::vector<int>& v)
  {
    return n >>= PushBackSink{v};
  }

  template<class... Ops>
  auto operator>>=(std::tuple<Ops...> n, std::vector<int>& v)
  {
    return n >>= PushBackSink{v};
  }

  void operator>>=(std::vector<int> const& source, Sink<int> auto sink)
  {
    for(const int i : source) { sink.push(i); }
  }
} // namespace pipes
