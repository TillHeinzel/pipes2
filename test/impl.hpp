#pragma once

namespace pipes
{
  template<class Op, class Next>
  struct Node
  {
    static constexpr bool isOpenSink = true;

    Op op;
    Next next;

    void push(int i) { op.push(next, i); }
  };

  template<class... Ops>
  struct RawNodes
  {
    RawNodes(Ops... ops) : ops(ops...) {}
    RawNodes(std::tuple<Ops...> ops) : ops(ops) {}

    std::tuple<Ops...> ops;
  };
} // namespace pipes

namespace pipes
{
  template<typename T, class I>
  concept Sink = requires(T t, I i) { t.push(i); };

  template<typename T>
  concept OpenSink = requires(T) { T::isOpenSink; };
} // namespace pipes

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
  auto addBefore(OpenSink auto s, RawNodes<Ops...> ops)
  {
    auto f = [&](auto... n) { return addBefore(s, n...); };
    return std::apply(f, ops.ops);
  }

  template<class... Ops1, class... Ops2>
  auto addBefore(RawNodes<Ops2...> ops2, RawNodes<Ops1...> ops1)
  {
    return RawNodes{std::tuple_cat(ops2.ops, ops1.ops)};
  }
} // namespace pipes
