#pragma once

#include "../Utility/RETURN.hpp"

namespace pipes::detail
{
  template<typename O, class Next, class T>
  concept Operation = requires(O op, Next& next, T t) { op.push(next, t); };

  template<class Op, class Next>
  struct Node
  {
    Op op;
    Next next;

    template<class T>
      requires(Operation<Op, Next, T>)
    void push(T t)
    {
      op.push(next, t);
    }
  };
} // namespace pipes::detail

#include "RawNodes.hpp"
#include "../Utility/reverse.hpp"

namespace pipes::detail
{
  auto connect_links_impl(auto s) -> decltype(s) { return s; }

  template<class Op, class... Ops>
  auto connect_links_impl(auto s, Op op, Ops... ops)
    PIPES_FWD(connect_links_impl(Node{op, s}, ops...));

  auto connect_links_f(auto s)
  {
    return [s](auto... n) { return connect_links_impl(s, n...); };
  }

  template<class... Ops>
  auto connect_links(RawNodes<Ops...> ops, auto s)
    PIPES_FWD(std::apply(connect_links_f(s), reverse(ops.ops)));
} // namespace pipes::detail
